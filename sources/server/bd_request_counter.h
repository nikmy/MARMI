#pragma once

#include <atomic>
#include <mutex>
#include "bd_request.h"

namespace server
{

class BDRequestCounter
{
 public:
    BDRequestCounter() = default;
    void inc(const BDRequest& r);
    void dec(const BDRequest& r);

    bool is_balanced() const;
    int64_t get_all();
    int64_t get_all_ignored() const;

 private:
    int64_t n_get;
    int64_t n_put;
    int64_t n_post;
    int64_t n_delete;
    int64_t total;
    
    std::mutex mutex;
};

}