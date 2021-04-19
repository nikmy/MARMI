#pragma once

#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>

namespace gen
{
using cond_var_t = std::condition_variable;
using thread_t = std::thread;
using mutex_t = std::mutex;
using lock_t = std::unique_lock<mutex_t>;

template <class T>
class Resource;

template <class T>
class DataHandler;

template <class T>
class ResourceManager;

enum Status
{
    STATUS_RUNNING,
    STATUS_STOPPED
};

}
