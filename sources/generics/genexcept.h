#pragma once

#include <stdexcept>

namespace gen
{

class IllegalAccess : public std::runtime_error
{
 public:
    explicit IllegalAccess(const char* info)
        : std::runtime_error(info)
    { }
};

class WaitingQueueOverflow : public std::runtime_error
{
 public:
    explicit WaitingQueueOverflow(const char* info)
        : std::runtime_error(info)
    { }
};

class UnsavedDataLeak : public std::runtime_error
{
 public:
    explicit UnsavedDataLeak(const char* info)
        : std::runtime_error(info)
    { }
};

}