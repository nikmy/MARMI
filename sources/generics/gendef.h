#pragma once

#include <condition_variable>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>

namespace gen
{
using conditional_variable_t = std::condition_variable;
using thread_t = std::thread;
using mutex_t = std::mutex;
using lock_t = std::unique_lock<mutex_t>;

template <typename X>
using func_t = std::function<X>;
using size_t = std::size_t;

template <typename T>
class Resource;

template <typename T>
class Handler;

template <typename T>
class Supplier;

template <typename T>
class ResourceManager;

enum Status
{
    STATUS_WAITING,
    STATUS_RUNNING,
    STATUS_STOPPED
};

}
