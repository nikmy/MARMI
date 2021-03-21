#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "progress_bar.h"
#include "manager.h"

using namespace gen;

class ResourceImpl
    : public Resource<int>
{
 public:
    static constexpr size_t CAP = 42;

    ResourceImpl()
        : v(CAP, 42)
    { }

    ~ResourceImpl() override
    { v.clear(); }

    int get_first()
    {
        int x = v.back();
        v.pop_back();
        return x;
    }

    int get_data() override
    { return get_first(); }

    bool is_empty() override
    { return v.empty(); }

 private:
    std::vector<int> v;
};

class HandlerImpl
    : public DataHandler<int>
{
 public:
    std::atomic_int popped;

    explicit HandlerImpl(ProgressBar& bar)
        : popped(0), bar_(bar)
    { }

    void process(int&& x) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        (--x)++;
        ++popped;

        std::lock_guard<std::mutex> guard(stream_mtx);
        bar_.make_progress();
        bar_.update();
    }

 private:
    ProgressBar& bar_;
    std::mutex  stream_mtx;
};

void test_correct_multithreading(size_t n_of_threads)
{
    ProgressBar bar(42);

    ResourceImpl container;
    HandlerImpl  handler(bar);

    ResourceManager<int> x(
        container,
        handler,
        64,
        n_of_threads
    );

    std::cout << "[+] Testing with " << n_of_threads << " threads:\n";

    bar.update();

    x.start();

    while (handler.popped < 42);

    x.stop();

    std::cout << std::endl;
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