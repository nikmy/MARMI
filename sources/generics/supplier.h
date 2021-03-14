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
            Handler<T>& handler,
            const func_t<void()>& supply_f,
            const func_t<bool()>& is_full_f
        );

    void start();
    void stop();

 private:
    SupplierStatus current_state_;
    Handler<T>& handler_;
    resource_t& target_;
    func_t<void()> supply_data_;
    func_t<bool()> is_full_;
    thread_t main_thread_;

    void run_();
};

template <class T>
Supplier<T>::Supplier
    (
        resource_t& target,
        Handler<T>& handler,
        const func_t<void()>& supply_f,
        const func_t<bool()>& is_full_f
    )
    : current_state_(STATUS_WAITING),
      handler_(handler),
      target_(target),
      supply_data_(supply_f),
      is_full_(is_full_f),
      main_thread_([&]() { run_(); })
{ }

template <class T>
void Supplier<T>::start()
{
    current_state_ = STATUS_RUNNING;
}

template <class T>
void Supplier<T>::stop()
{
    current_state_ = STATUS_STOPPED;
    main_thread_.join();
}

template <class T>
void Supplier<T>::run_()
{
    while (current_state_ == STATUS_WAITING);
    while (current_state_ == STATUS_RUNNING) {
        supply_data_();
        handler_.awake_one();
        while (is_full_()) {
            handler_.awake_all();
        }
    }
}

}