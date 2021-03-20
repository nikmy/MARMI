#include "task_scheduler.h"

namespace ts
{

TaskScheduler::TaskScheduler(
    size_t n_of_threads,
    size_t max_size
)
    : queue_(max_size),
      manager(
          queue_,
          [&] { add_job(); },
          [](const task_t& job) { job(); },
          [&] { finish(); },
          []() { },
          n_of_threads,
          1, true
      )
{ }

TaskScheduler::~TaskScheduler()
{ manager.stop(); }

void TaskScheduler::launch()
{ manager.start(); }

void TaskScheduler::shutdown()
{ manager.stop(); }

void TaskScheduler::add_job()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(6));
    queue_.emplace([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(24));
    });
}

TaskScheduler& GetScheduler()
{
    static TaskScheduler scheduler = TaskScheduler(2, 1024);
    return scheduler;
}

void TaskScheduler::finish()
{
    while (!queue_.is_empty()) {
        queue_.get_data()();
    }
}
}