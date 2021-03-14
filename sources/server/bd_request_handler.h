#pragma once

#include <thread>

#include "bd_request.h"
#include "bd_request_counter.h"

namespace server
{

using BDResponse = RData;

class BDRequestHandler
{
 public:
    explicit BDRequestHandler(BDRequestCounter& c);
    void process(BDRequest request);

 private:
    BDRequestCounter& counter_;
};

}