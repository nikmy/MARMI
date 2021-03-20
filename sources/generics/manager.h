#pragma once

#include "queue.h"
#include "gendef.h"
#include "resource.h"

namespace gen
{

template <class T>
class ResourceManager
{
 public:
    using resource_t = Resource<T>;
    using data_t = T;

    ResourceManager
        (
            resource_t& resource
        ) = delete;

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

 private:
    resource_t& resource_;
    mutex_t               resource_mutex_;
    func_t<void(data_t)>        process_data_;
    std::vector<thread_t> workers_;

    Queue<data_t> queue_;
    mutex_t       queue_mutex_;

    cond_var_t cv_run_;
    cond_var_t cv_put_;

    Status current_state_;

    void put_data_in_queue_();
    void worker_routine_();
};

template <class T>
void ResourceManager<T>::put_data_in_queue_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t lock(resource_mutex_);
        cv_put_.wait(
            lock,
            [&] { return !queue_.full() && current_state_ != STATUS_WAITING; }
        );

        if (current_state_ == STATUS_RUNNING && !queue_.full()) {
            data_t data = resource_.get_data();
            queue_.push(data);
            cv_run_.notify_one();
        }
    }
}

template <class T>
void ResourceManager<T>::worker_routine_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t lock(queue_mutex_);

        cv_run_.wait(
            lock,
            [&] {
                return current_state_ != STATUS_WAITING;
            }
        );

        cv_run_.wait(
            lock,
            [&] {
                return !queue_.empty() || current_state_ == STATUS_STOPPED;
            }
        );

        if (current_state_ == STATUS_RUNNING) {
            data_t data = queue_.take_first();
            cv_put_.notify_one();
            queue_mutex_.unlock();
            process_data_(data);
        }
    }
}

}
