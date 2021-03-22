#pragma once

#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>

#include "genexcept.h"

namespace gen
{
using cond_var_t = std::condition_variable;
using thread_t = std::thread;
using mutex_t = std::mutex;
using lock_t = std::unique_lock<mutex_t>;

template <class X>
using func_t = std::function<X>;
using size_t = unsigned long;

template <class T>
class Resource;

template <class T>
class DataHandler;

template <class T>
struct ShutdownStrategy;

template <class T>
class ResourceManager;

enum Status
{
    STATUS_WAITING,
    STATUS_RUNNING,
    STATUS_STOPPED
};

}
