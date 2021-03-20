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

class TaskScheduler
{
 public:
    friend TaskScheduler& GetScheduler();

    TaskScheduler() = delete;
    TaskScheduler(const TaskScheduler&) = delete;

    ~TaskScheduler();

    void add_job();
    void finish();
    void launch();
    void shutdown();

 private:
    mtq::Queue<task_t>    queue_;
    gen::ResourceManager<task_t> manager;

    explicit TaskScheduler(size_t n_of_threads, size_t max_size);
};

TaskScheduler& GetScheduler();

}