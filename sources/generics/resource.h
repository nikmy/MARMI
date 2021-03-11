#pragma once

#include "gendef.h"

namespace gen
{

template <class T>
class Resource
{
    friend class Supplier<T>;

    friend class Handler<T>;

 public:
    Resource() = default;
    Resource(const Resource&);
    ~Resource() = default;

 private:
    mutex_t access_mutex_;
};

template <class T>
Resource<T>::Resource(const Resource&)
    : Resource()
{ }

}
