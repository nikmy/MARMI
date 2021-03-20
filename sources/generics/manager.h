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
            resource_t& resource,
            const func_t<void()>& supply_f,
            const func_t<void(const data_t&)>& handle_f,
            const func_t<void()>& save_session_f,
            const func_t<void()>& restore_session_f,
            size_t n_of_workers,
            size_t n_of_employers,
            bool parallel_processing = false
        );

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

    void start();
    void stop();

 private:
    resource_t& resource_;
    mutex_t access_mutex_;
    mutex_t supply_mutex_;

    func_t<void()>              supply_;
    func_t<void(const data_t&)> handle_;

    func_t<void()> save_session_;
    func_t<void()> restore_session_;

    size_t n_of_workers_;
    size_t n_of_employers_;

    Status handler_state_;
    Status supplier_state_;

    cond_var_t is_not_empty_;
    cond_var_t is_not_full_;

    std::vector<thread_t> workers_;
    std::vector<thread_t> employers_;

    bool parallel_processing_;

    void employer_routine_();
    void worker_routine_();
};

template <class T>
ResourceManager<T>::ResourceManager(
    resource_t& resource,
    const func_t<void()>& supply_f,
    const func_t<void(const data_t&)>& handle_f,
    const func_t<void()>& save_session_f,
    const func_t<void()>& restore_session_f,
    size_t n_of_workers,
    size_t n_of_employers,
    bool parallel_processing
)
    : resource_(resource),
      access_mutex_(),
      supply_mutex_(),
      supply_(supply_f),
      handle_(handle_f),
      save_session_(save_session_f),
      restore_session_(restore_session_f),
      n_of_workers_(n_of_workers),
      n_of_employers_(n_of_employers),
      handler_state_(STATUS_WAITING),
      supplier_state_(STATUS_WAITING),
      is_not_empty_(),
      is_not_full_(),
      workers_(),
      employers_(),
      parallel_processing_(parallel_processing)
{ }

template <class T>
void ResourceManager<T>::start()
{
    restore_session_();

    workers_.reserve(n_of_workers_);
    employers_.reserve(n_of_employers_);

    for (size_t i = 0; i < n_of_workers_; ++i) {
        workers_.emplace_back([&]() { worker_routine_(); });
    }

    for (size_t i = 0; i < n_of_employers_; ++i) {
        employers_.emplace_back([&]() { employer_routine_(); });
    }

    handler_state_  = STATUS_RUNNING;
    supplier_state_ = STATUS_RUNNING;
}

template <class T>
void ResourceManager<T>::stop()
{
    if (supplier_state_ == STATUS_STOPPED && handler_state_ == STATUS_STOPPED) {
        return;
    }

    supplier_state_ = STATUS_STOPPED;
    handler_state_  = STATUS_STOPPED;

    is_not_empty_.notify_all();
    is_not_full_.notify_all();

    for (auto& t : employers_)
        t.join();
    for (auto& t : workers_)
        t.join();

    employers_.clear();
    workers_.clear();

    save_session_();
}

template <class T>
void ResourceManager<T>::employer_routine_()
{
    while (supplier_state_ == STATUS_WAITING);
    while (supplier_state_ == STATUS_RUNNING) {
        lock_t lock(supply_mutex_);
        is_not_full_.wait(
            lock,
            [&] {
                return !resource_.is_full()
                       || supplier_state_ == STATUS_STOPPED;
            }
        );

        if (supplier_state_ == STATUS_RUNNING) {
            supply_();
            is_not_empty_.notify_one();
        }
    }
}

template <class T>
void ResourceManager<T>::worker_routine_()
{
    while (handler_state_ == STATUS_WAITING);
    while (handler_state_ == STATUS_RUNNING) {
        lock_t data_lock(access_mutex_);
        is_not_empty_.wait(
            data_lock,
            [&] {
                return !resource_.is_empty()
                       || handler_state_ == STATUS_STOPPED;
            }
        );

        if (handler_state_ == STATUS_RUNNING) {
            data_t item = resource_.get_data();
            is_not_full_.notify_one();

            if (parallel_processing_)
                data_lock.unlock();

            handle_(item);
        }
    }
}

}
