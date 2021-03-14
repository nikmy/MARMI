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
            const func_t<bool()>& is_empty_f,
            const func_t<bool()>& is_full_f,
            size_t n_of_threads_limit
        );

    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;

    virtual void launch();
    virtual void terminate();

 private:
    handler_t handler_;
    supplier_t supplier_;
};

template <class T>
ResourceManager<T>::ResourceManager
    (
        ResourceManager::resource_t& resource,
        const func_t<void()>& supply_f,
        const func_t<data_t(void)>& access_f,
        const func_t<void(data_t)>& handle_f,
        const func_t<bool()>& is_empty_f,
        const func_t<bool()>& is_full_f,
        size_t n_of_threads_limit
    )
    : handler_(
          resource,
          access_f,
          handle_f,
          is_empty_f,
          n_of_threads_limit - 3
      ),
      supplier_(
          resource,
          handler_,
          supply_f,
          is_full_f
      )
{
}

template <class T>
void ResourceManager<T>::launch()
{
    handler_.start();
    supplier_.start();
}

template <class T>
void ResourceManager<T>::terminate()
{
    supplier_.stop();
    handler_.stop();
}

}