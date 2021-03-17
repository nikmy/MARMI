#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "progress_bar.h"
#include "manager.h"

using namespace gen;

template <typename T>
class ResourceImpl
    : public Resource<T>
{
 public:
    static constexpr size_t CAP = 42;

    std::atomic_int pushed;
    std::atomic_int popped;

    ResourceImpl()
    {
        pushed = 0;
        popped = 0;
        v.reserve(CAP);
    }

    T get_first()
    {
        T x = v.back();
        v.pop_back();
        return x;
    }

    template <typename...Args>
    void emplace_back(Args&& ...args)
    {
        v.emplace_back(std::forward<Args>(args)...);
    }

    T get_data() override
    {
        return get_first();
    }

    bool is_full() override
    {
        return v.size() == CAP;
    }

    bool is_empty() override
    {
        return v.empty();
    }

 private:
    std::vector<T> v;
};

gen::mutex_t stream_mtx;

ProgressBar bar(42);

void test_correct_concurrency(size_t n_of_threads)
{
    ResourceImpl<int> container;

    auto handle_data = [&container](int x) {
        (--x)++;

        gen::lock_t lock(stream_mtx);

        container.popped++;

        ::bar.make_progress();
        ::bar.update();

        stream_mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    };

    auto supply_data = [&container]() {
        if (container.pushed < 42) {
            container.pushed++;
            container.emplace_back(42);
        }
    };

    ResourceManager<int> x(
        container,
        supply_data,
        handle_data,
        [] { },
        [] { },
        n_of_threads - n_of_threads / 2,
        n_of_threads / 2);

    std::cout << "[+] Test concurrency with " << n_of_threads << " threads:\n";

    bar.update();

    x.start();

    while (container.pushed < 42);
    while (container.popped < 42);

    x.stop();

    std::cout << std::endl;

    bar.assign(42);

}

void test_generics()
{
    std::cout << "[INFO] GenericsTest is running..." << std::endl;

    test_correct_concurrency(4);
    test_correct_concurrency(8);
    test_correct_concurrency(16);
    test_correct_concurrency(32);

    std::cout << std::endl;
}