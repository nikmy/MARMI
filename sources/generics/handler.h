#pragma once

#include "gendef.h"

namespace gen
{

template <class T>
class DataHandler
{
 public:
    using data_t = T;

    DataHandler() = default;
    DataHandler(const DataHandler&) = default;

    virtual void process(data_t&& data) = 0;
    virtual ~DataHandler() = default;
};

}