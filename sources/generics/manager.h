#pragma once

#include "queue.h"
#include "gendef.h"
#include "resource.h"
#include "handler.h"

namespace gen
{

template <class T>
class ResourceManager
{
 public:
    using resource_t = Resource<T>;
    using handler_t = DataHandler<T>;
    using data_t = T;

    ResourceManager
        (
            resource_t& resource,
            handler_t& handler,
            size_t max_queue_size,
            size_t n_of_threads
        );

    ~ResourceManager();

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

    void start();
    void stop();

    void save_session_data(Queue<T>& backup);
    void restore_session_data(Queue<T>& backup);

 private:
    resource_t& resource_;
    handler_t & handler_;

    std::vector<thread_t> threads_;
    size_t                n_of_threads_;

    Queue<data_t> queue_;

    mutex_t resource_mutex_;
    mutex_t queue_mutex_;

    cond_var_t cv_run_;
    cond_var_t cv_put_;

    Status current_state_;

    void receive_data_();
    void process_data_();
};

template <class T>
ResourceManager<T>::ResourceManager(
    resource_t& resource,
    handler_t& handler,
    size_t max_queue_size,
    size_t n_of_threads
)
    : resource_(resource),
      handler_(handler),
      n_of_threads_(n_of_threads),
      queue_(max_queue_size),
      current_state_(STATUS_STOPPED)
{ threads_.reserve(n_of_threads); }

template <class T>
ResourceManager<T>::~ResourceManager()
{ stop(); }

template <class T>
void ResourceManager<T>::start()
{
    current_state_ = STATUS_RUNNING;

    threads_.template emplace_back([&] { receive_data_(); });

    for (size_t i = 1; i < n_of_threads_; ++i) {
        threads_.template emplace_back([&] { process_data_(); });
    }

    cv_put_.notify_one();
    cv_run_.notify_all();
}

template <class T>
void ResourceManager<T>::stop()
{
    current_state_ = STATUS_STOPPED;
    cv_put_.notify_one();
    cv_run_.notify_all();

    for (auto& t : threads_) {
        t.join();
    }

    threads_.clear();
}

template <class T>
void ResourceManager<T>::receive_data_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t lock(resource_mutex_);

        while (resource_.is_empty() && current_state_ != STATUS_STOPPED) { }

        cv_put_.wait(
            lock,
            [&] { return !queue_.full() || current_state_ == STATUS_STOPPED; }
        );

        if (current_state_ == STATUS_RUNNING && !resource_.is_empty()) {
            queue_.template emplace(std::move(resource_.get_data()));
            cv_run_.notify_one();
        }
    }
}

template <class T>
void ResourceManager<T>::process_data_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t lock(queue_mutex_);
        cv_run_.wait(
            lock,
            [&] { return !queue_.empty() || current_state_ == STATUS_STOPPED; }
        );
        lock.unlock();

        if (!queue_.empty() && current_state_ == STATUS_RUNNING) {
            handler_.process(std::move(queue_.take_first()));
            cv_put_.notify_one();
        }
    }
}

template <class T>
void ResourceManager<T>::save_session_data(gen::Queue<T>& backup)
{
    if (current_state_ == STATUS_RUNNING) {
        stop();
    }
    queue_.move_to(backup);
}

template <class T>
void ResourceManager<T>::restore_session_data(gen::Queue<T>& backup)
{
    if (current_state_ == STATUS_RUNNING) {
        stop();
    }
    backup.move_to(queue_);
}

}
