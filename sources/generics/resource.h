#pragma once

#include "gendef.h"

namespace gen
{

template <class T>
class Resource
{
    friend class Supplier<T>;

    friend class Handler<T>;

    friend class ResourceManager<T>;

 public:
    Resource() = default;
    Resource(const Resource&);
    virtual ~Resource() = default;

    virtual void put_data() = 0;
    virtual T get_data() = 0;
    virtual bool is_empty() = 0;
};

template <class T>
Resource<T>::Resource(const Resource&)
    : Resource()
{ }

}
