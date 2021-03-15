#pragma once

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
            resource_t                  & resource,
            const func_t<void()>        & supply_f,
            const func_t<void(data_t)>  & handle_f,
            const func_t<data_t(void)>  & take_f,
            const func_t<void()>        & save_f,
            const func_t<bool()>        & is_empty_f,
            const func_t<bool()>        & is_full_f,
            size_t                      n_of_workers,
            size_t                      n_of_consumers
        );

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

    void start();
    void stop();

 private:
    resource_t& resource_;

    func_t<void()>       supply_;
    func_t<void(data_t)> handle_;
    func_t<data_t(void)> take_;
    func_t<void()>       save_;
    func_t<bool()>       is_empty_;
    func_t<bool()>       is_full_;

    size_t n_of_workers_;
    size_t n_of_consumers_;

    Status handler_state_;
    Status supplier_state_;

    conditional_variable_t is_not_empty_;
    conditional_variable_t is_not_full_;

    std::vector<thread_t> workers_;
    std::vector<thread_t> consumers_;

    [[noreturn]] void consumer_routine();
    [[noreturn]] void worker_routine();
};

template <class T>
ResourceManager<T>::ResourceManager(
    resource_t& resource,
    const func_t<void()>& supply_f,
    const func_t<void(data_t)>& handle_f,
    const func_t<data_t(void)>& take_f,
    const func_t<void()>& save_f,
    const func_t<bool()>& is_empty_f,
    const func_t<bool()>& is_full_f,
    size_t n_of_workers,
    size_t n_of_consumers
)
    : resource_(resource),
      supply_(supply_f),
      handle_(handle_f),
      take_(take_f),
      save_(save_f),
      is_empty_(is_empty_f),
      is_full_(is_full_f),
      n_of_workers_(n_of_workers),
      n_of_consumers_(n_of_consumers),
      handler_state_(STATUS_WAITING),
      supplier_state_(STATUS_WAITING),
      is_not_empty_(),
      is_not_full_(),
      workers_(),
      consumers_()
{ }

template <class T>
void ResourceManager<T>::start()
{
    for (size_t i = 0; i < n_of_workers_; ++i) {
        workers_.emplace_back([&]() { worker_routine(); });
    }

    for (size_t i = 0; i < n_of_consumers_; ++i) {
        consumers_.emplace_back([&]() { consumer_routine(); });
    }

    handler_state_  = STATUS_RUNNING;
    supplier_state_ = STATUS_RUNNING;
}

template <class T>
void ResourceManager<T>::stop()
{
    supplier_state_ = STATUS_STOPPED;
    handler_state_  = STATUS_STOPPED;

    is_not_empty_.notify_all();
    is_not_full_.notify_all();

    save_();

    consumers_.clear();
    workers_.clear();
}

template <class T>
[[noreturn]] void ResourceManager<T>::consumer_routine()
{
    while (supplier_state_ == STATUS_WAITING);
    while (supplier_state_ == STATUS_RUNNING) {
        lock_t lock(resource_.access_mutex_);
        is_not_full_.wait(
            lock,
            [&] { return !is_full_() || supplier_state_ == STATUS_STOPPED; }
        );

        if (supplier_state_ == STATUS_RUNNING) {
            supply_();
            is_not_empty_.notify_one();
        }
    }
}

template <class T>
[[noreturn]] void ResourceManager<T>::worker_routine()
{
    while (handler_state_ == STATUS_WAITING);
    while (handler_state_ == STATUS_RUNNING) {
        lock_t data_lock(resource_.access_mutex_);
        is_not_empty_.wait(
            data_lock,
            [&] { return !is_empty_() || handler_state_ == STATUS_STOPPED; }
        );

        if (handler_state_ == STATUS_RUNNING) {
            data_t item = take_();
            is_not_full_.notify_one();
            data_lock.unlock();
            handle_(item);
        }
    }
}

}
