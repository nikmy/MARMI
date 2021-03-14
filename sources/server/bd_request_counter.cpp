#include "bd_request_counter.h"

namespace server
{

void BDRequestCounter::inc(const BDRequest& r)
{
//    mutex.lock();
    std::string t = r.getData().txt;
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
//    mutex.unlock();
}

void BDRequestCounter::dec(const BDRequest& r)
{
//    mutex.lock();
    std::string t = r.getData().txt;
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
//    mutex.unlock();
}

bool BDRequestCounter::is_balanced() const
{
    return (n_get == 0 && n_post == 0 && n_put == 0 && n_delete == 0);
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