#pragma once

#include "gendef.h"
#include "resource.h"
#include "handler.h"
#include "supplier.h"

namespace gen
{

template <class T>
class ResourceManager
{
 public:
    using resource_t = Resource<T>;
    using supplier_t = Supplier<T>;
    using handler_t = Handler<T>;
    using data_t = T;

    ResourceManager
        (
            resource_t& resource,
            const func_t<void()>& supply_f,
            const func_t<data_t(void)>& access_f,
            const func_t<void(data_t)>& handle_f,
            const func_t<bool()>& is_full,
            const func_t<bool()>& is_empty,
            size_t n_of_threads_limit
        );

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

    virtual void launch();
    virtual void terminate();

 private:
    resource_t& resource_;
    supplier_t supplier_;
    handler_t handler_;
};

template <class T>
ResourceManager<T>::ResourceManager
    (
        ResourceManager::resource_t& resource,
        const func_t<void()>& supply_f,
        const func_t<data_t(void)>& access_f,
        const func_t<void(data_t)>& handle_f,
        const func_t<bool()>& is_full,
        const func_t<bool()>& is_empty,
        size_t n_of_threads_limit
    )
    : resource_(resource),
      supplier_(resource, supply_f, is_full),
      handler_(resource, access_f, handle_f, is_empty, n_of_threads_limit - 1)
{ }

template <class T>
void ResourceManager<T>::launch()
{
    supplier_.start();
    handler_.start();
}

template <class T>
void ResourceManager<T>::terminate()
{
    supplier_.stop();
    handler_.stop();
}

}