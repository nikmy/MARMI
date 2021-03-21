#pragma once

#include "gendef.h"

namespace gen
{

template <class T>
class Resource
{
 public:
    Resource() = default;
    Resource(const Resource&) = default;
    virtual ~Resource() = default;

    virtual T get_data() = 0;
    virtual bool is_empty() = 0;
};

}
