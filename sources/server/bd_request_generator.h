#pragma once

#include "bd_request.h"
#include "bd_request_counter.h"

namespace server
{

class BDRequestGenerator
{
 public:
    explicit BDRequestGenerator(BDRequestCounter& c);
    BDRequest getRequest();

 private:
    BDRequestCounter& counter_;
};

}