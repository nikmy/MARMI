#pragma once

#include <vector>

#include "queue.h"
#include "manager.h"

namespace ts
{

using cond_var_t = gen::cond_var_t;
using task_t = gen::func_t<void()>;
using size_t = gen::size_t;
using thread_t = gen::thread_t;
using mutex_t = gen::mutex_t;

class ThreadPool
{
 public:
    ThreadPool() = delete;
    ThreadPool(const ThreadPool&) = delete;

    ~ThreadPool();

    void add_job();
    void launch();
    void shutdown();

 private:
    mtq::Queue<task_t> queue_;
    std::vector<thread_t> workers_;
    cond_var_t is_not_empty;
    mutex_t access_mutex_;
    bool terminate_pool_;
    bool stopped_;

    explicit ThreadPool(size_t n_of_threads, size_t max_size);

    [[noreturn]] void routine_();
};

}