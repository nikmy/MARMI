#include "bd_request_handler.h"

namespace server
{

BDRequestHandler::BDRequestHandler(BDRequestCounter& c)
    : counter_(c)
{ }

void BDRequestHandler::process(BDRequest&& request)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    counter_.dec(request);
}

}