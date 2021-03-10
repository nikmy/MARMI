#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "manager.h"

using namespace gen;

template <typename T>
class ResourceImpl : public Resource<T>
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
        return v.size() == CAP;
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

void handle_data(int x)
{
    (--x)++;
    gen::lock_t lock(stream_mtx);
    std::cout << '#';
    std::flush(std::cout);
    stream_mtx.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

decltype(auto) test_correct_multithreading(size_t n_of_threads)
{
    ResourceImpl<int> container;

    auto get_data = [&]() { return container.get_first(); };
    auto is_full_f = [&]() { return container.is_full(); };
    auto is_empty_f = [&]() { return container.is_empty(); };
    auto supply_data = [&]() { container.emplace_back(42); };

    ResourceManager<int> x(
        container,
        supply_data,
        get_data,
        handle_data,
        is_full_f,
        is_empty_f,
        n_of_threads);

    std::cout << n_of_threads << ((n_of_threads / 10) ? " " : "  ") << " threads: [";

    auto b = std::chrono::steady_clock::now();
    x.launch();
    while (!container.is_empty());
    x.terminate();
    auto e = std::chrono::steady_clock::now();

    std::cout << "]" << std::endl;

    return std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
}

void test_generics()
{
    std::cout << "[INFO] GenericsTest is running..." << std::endl;
    test_correct_multithreading(2);
    test_correct_multithreading(8);
    test_correct_multithreading(16);
    test_correct_multithreading(32);
    test_correct_multithreading(43);
    std::cout << "[OK] All tests completed\n" << std::endl;
}