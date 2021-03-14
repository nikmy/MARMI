#include "bd_request_generator.h"

#include <ctime>

namespace server
{

BDRequestGenerator::BDRequestGenerator(BDRequestCounter& c)
    : counter_(c)
{ }

BDRequest BDRequestGenerator::getRequest()
{
    static size_t id = 0;
    ++id;

    long code;
    std::time(&code);
    code %= 4;

    BDRequest r("", 0);

    switch (code) {
        case 0:
            r = BDRequest("GET", id);
            break;
        case 1:
            r = BDRequest("POST", id);
            break;
        case 2:
            r = BDRequest("PUT", id);
            break;
        default:
            r = BDRequest("DELETE", id);
    }

    counter_.inc(r);
    return r;
}

}