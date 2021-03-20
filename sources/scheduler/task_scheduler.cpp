#include "thread_pool.h"

namespace ts
{

ThreadPool::ThreadPool(size_t n_of_threads, size_t max_size)
    : queue_(max_size)
{
    workers_.reserve(n_of_threads);
    terminate_pool_ = false;
    stopped_        = true;
}

ThreadPool::~ThreadPool()
{
    if (!stopped_)
        shutdown();
    workers_.clear();
}

void ThreadPool::launch()
{
    for (size_t i = 0; i < workers_.max_size(); ++i) {
        workers_.emplace_back([&] { routine_(); });
    }
    stopped_ = false;
}

void ThreadPool::shutdown()
{
    for (auto& t : workers_)
        t.join();
    stopped_ = true;
}

void ThreadPool::add_job()
{

}

}