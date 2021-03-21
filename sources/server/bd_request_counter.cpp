#include "bd_request_counter.h"

namespace server
{

void BDRequestCounter::inc(const BDRequest& r)
{
    std::string t = r.getData().txt;
    std::lock_guard<std::mutex> guard(mutex);
    if (t == "GET") {
        ++n_get;
    }
    else if (t == "PUT") {
        ++n_put;
    }
    else if (t == "POST") {
        ++n_post;
    }
    else if (t == "DELETE") {
        ++n_delete;
    }
    ++total;
}

void BDRequestCounter::dec(const BDRequest& r)
{
    std::string t = r.getData().txt;
    std::lock_guard<std::mutex> guard(mutex);
    if (t == "GET") {
        --n_get;
    }
    else if (t == "PUT") {
        --n_put;
    }
    else if (t == "POST") {
        --n_post;
    }
    else if (t == "DELETE") {
        --n_delete;
    }
}

int64_t BDRequestCounter::get_all_ignored() const
{
    return n_get + n_put + n_post + n_delete;
}

int64_t BDRequestCounter::get_all()
{
    return total;
}

}