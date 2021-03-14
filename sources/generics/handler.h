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
            const func_t<bool()>& is_empty_f,
            size_t n_of_threads
        );

    void start();
    void await();
    void awake_one();
    void awake_all();
    void stop();

 private:
    HandlerStatus current_state_;
    conditional_variable_t work_;
    resource_t& resource_;
    func_t<data_t(void)> get_data_;
    func_t<void(data_t)> handle_;
    func_t<bool()> is_empty_;
    size_t n_of_threads_;
    std::vector<thread_t> threads_;

    void run_();
};

template <class T>
Handler<T>::Handler
    (
        resource_t& resource,
        const func_t<data_t(void)>& access_f,
        const func_t<void(data_t)>& handle_f,
        const func_t<bool()>& is_empty_f,
        size_t n_of_threads
    )
    : current_state_(STATUS_WAITING),
      resource_(resource),
      get_data_(access_f),
      handle_(handle_f),
      is_empty_(is_empty_f),
      n_of_threads_(n_of_threads),
      threads_()
{ }

template <class T>
void Handler<T>::start()
{
    for (size_t i = 0; i < n_of_threads_; ++i) {
        threads_.emplace_back([&]() { run_(); });
    }
}

template <class T>
void Handler<T>::await()
{
    current_state_ = STATUS_WAITING;
}

template <class T>
void Handler<T>::awake_one()
{
    current_state_= STATUS_RUNNING;
    work_.notify_one();
}


template <class T>
void Handler<T>::awake_all()
{
    current_state_= STATUS_RUNNING;
    work_.notify_all();
}


// Undefined behaviour
//------------------------------------------------------------------------------
template <class T>
void Handler<T>::stop()
{
    while (!is_empty_()) {
        awake_one();
    }

    current_state_ = STATUS_STOPPED;
    work_.notify_all();
    for (auto& t : threads_)
        t.join();
}

template <class T>
void Handler<T>::run_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t data_lock(resource_.access_mutex_);
        work_.wait(
            data_lock,
            [&] { return current_state_ != STATUS_WAITING; }
        );

        if (!is_empty_()) {
            data_t item = get_data_();
            data_lock.unlock();
            handle_(item);
        }
    }
}
//------------------------------------------------------------------------------
}