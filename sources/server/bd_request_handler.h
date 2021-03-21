#pragma once

#include "handler.h"
#include "bd_request.h"
#include "bd_request_counter.h"

namespace server
{

using BDResponse = RData;

class BDRequestHandler : public gen::DataHandler<BDRequest>
{
 public:
    explicit BDRequestHandler(BDRequestCounter& c);

 private:
    void process(BDRequest&& data) override;

 private:
    BDRequestCounter& counter_;
};

}