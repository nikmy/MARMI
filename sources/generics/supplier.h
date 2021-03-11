#pragma once

#include "gendef.h"
#include "resource.h"

namespace gen
{

template <class T>
class Supplier
{
 public:
    friend class ResourceManager<T>;

    using resource_t = Resource<T>;
    using SupplierStatus = Status;

    Supplier
        (
            resource_t& target,
            const func_t<void()>& supply_f,
            const func_t<bool()>& need_wait_f
        );

    void start();
    void stop();

 private:
    resource_t& target_;
    func_t<void()> supply_data_;
    func_t<bool()> need_wait_;
    std::vector<thread_t> main_thread_;
    SupplierStatus current_state_;
    conditional_variable_t cv_status_;

    void run_();
};

template <class T>
Supplier<T>::Supplier
    (
        resource_t& target,
        const func_t<void()>& supply_f,
        const func_t<bool()>& need_wait_f
    )
    : target_(target),
      supply_data_(supply_f),
      need_wait_(need_wait_f),
      main_thread_(),
      current_state_(STATUS_WAITING),
      cv_status_()
{ }

template <class T>
void Supplier<T>::start()
{
    main_thread_.emplace_back([&]() { run_(); });
}

template <class T>
void Supplier<T>::stop()
{
    current_state_ = STATUS_STOPPED;
    cv_status_.notify_one();
    main_thread_[0].join();
}

template <class T>
void Supplier<T>::run_()
{
    while (current_state_ != STATUS_STOPPED) {
        lock_t lock(target_.access_mutex_);
        if (need_wait_()) {
            current_state_ = STATUS_WAITING;
        }

        cv_status_.wait(
            lock,
            [&] {
                if (current_state_ == STATUS_STOPPED)
                    return true;
                if (!need_wait_())
                    current_state_ = STATUS_RUNNING;
                return current_state_ != STATUS_WAITING;
            }
        );

        if (current_state_ == STATUS_RUNNING) {
            supply_data_();
        }
    }
}

}
