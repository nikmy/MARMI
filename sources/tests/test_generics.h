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
    ResourceImpl()
    {
        v.reserve(CAP);
    }

    T get_first()
    {
        int x = v.back();
        v.pop_back();
        return x;
    }

    template <typename...Args>
    void emplace_back(Args&& ...args)
    {
        v.emplace_back(std::forward<Args>(args)...);
    }

    bool is_full()
    {
        return v.size() == v.max_size();
    }

    bool is_empty()
    {
        return v.empty();
    }

 private:
    std::vector<T> v;
    static constexpr size_t CAP = 42;
};

gen::mutex_t stream_mtx;

ProgressBar bar(42);

void handle_data(int x)
{
    (--x)++;

    volatile gen::lock_t lock(stream_mtx);

    ::bar.make_progress();
    ::bar.update();

    stream_mtx.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

decltype(auto) test_correct_multithreading(size_t n_of_threads)
{
    ResourceImpl<int> container;

    std::atomic<int> pushed = 0;
    std::atomic<int> popped = 0;

    auto get_data = [&popped, &container]() {
        ++popped;
        return container.get_first();
    };

    auto supply_data = [&pushed, &container]() {
        if (++pushed <= 43) {
            container.emplace_back(42);
        }
    };

    auto is_full_f = [&container]() { return container.is_full(); };
    auto is_empty_f = [&container]() { return container.is_empty(); };

    ResourceManager<int> x(
        container,
        supply_data,
        get_data,
        handle_data,
        is_empty_f,
        is_full_f,
        n_of_threads);


    std::cout << n_of_threads << ((n_of_threads / 10) ? " " : "  ")
              << " threads:\n";

    auto b = std::chrono::steady_clock::now();
    x.start();

    while (pushed < 43);
    while (popped < 42);

    x.stop();

    auto e = std::chrono::steady_clock::now();

    std::cout << std::endl;

    bar.assign(42);

    return std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
}

void test_generics()
{
    std::cout << "[INFO] GenericsTest is running..." << std::endl;
    test_correct_multithreading(4);
    test_correct_multithreading(8);
    test_correct_multithreading(16);
    test_correct_multithreading(32);
    test_correct_multithreading(43);
    std::cout << std::endl;
}