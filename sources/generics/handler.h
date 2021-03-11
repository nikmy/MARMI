#pragma once

#include "gendef.h"
#include "resource.h"

namespace gen
{

template <class T>
class Handler
{
 public:
    using HandlerStatus = std::atomic<Status>;
    using resource_t = Resource<T>;
    using data_t = T;

    Handler() = delete;
    Handler(const Handler&) = delete;

    Handler
        (
            resource_t& resource,
            const func_t<data_t(void)>& access_f,
            const func_t<void(data_t)>& handle_f,
            const func_t<bool()>& need_wait_f,
            size_t n_of_threads
        );

    void start();
    void stop();

 private:
    resource_t& resource_;
    func_t<data_t(void)> get_data_;
    func_t<void(data_t)> handle_;
    func_t<bool()> need_wait_;
    size_t n_of_threads_;
    std::vector<thread_t> threads_;
    HandlerStatus current_state_;
    conditional_variable_t cv_status_;

    void run_();
};

template <class T>
Handler<T>::Handler
    (
        resource_t& resource,
        const func_t<data_t(void)>& access_f,
        const func_t<void(data_t)>& handle_f,
        const func_t<bool()>& need_wait_f,
        size_t n_of_threads
    )
    : resource_(resource),
      get_data_(access_f),
      handle_(handle_f),
      need_wait_(need_wait_f),
      n_of_threads_(n_of_threads),
      threads_(),
      current_state_(STATUS_WAITING),
      cv_status_()
{}

template <class T>
void Handler<T>::start()
{
    for (size_t i = 0; i < n_of_threads_; ++i)
        threads_.emplace_back([&]() { run_(); });
    current_state_ = STATUS_RUNNING;
}

template <class T>
void Handler<T>::stop()
{
    current_state_ = STATUS_STOPPED;
    cv_status_.notify_all();
    for (auto& t : threads_)
        t.join();
}

template <class T>
void Handler<T>::run_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t data_lock(resource_.access_mutex_);
        if (need_wait_()) {
            current_state_ = STATUS_WAITING;
        }

        cv_status_.wait(
            data_lock,
            [&] {
                if (current_state_ == STATUS_STOPPED) return true;
                if (!need_wait_()) current_state_ = STATUS_RUNNING;
                return current_state_ != STATUS_WAITING;
            }
        );

        if (current_state_ == STATUS_RUNNING) {
            const data_t& item = get_data_();
            data_lock.unlock();
            handle_(item);
        }
    }
}

}
