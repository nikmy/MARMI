#pragma once

#include "resource.h"
#include "bd_request.h"
#include "bd_request_counter.h"

namespace server
{

 class BDRequestGenerator : public gen::Resource<BDRequest>
{
 public:
    explicit BDRequestGenerator(BDRequestCounter& c);

     ~BDRequestGenerator() override = default;
     BDRequest get_data() override;
     bool is_empty() override;

  private:
    BDRequestCounter& counter_;
};

}