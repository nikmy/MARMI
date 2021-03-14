#pragma once

#include "queue.h"
#include "manager.h"

#include "bd_request.h"
#include "bd_request_handler.h"
#include "bd_request_generator.h"

namespace server
{

class EchoServer
{
 public:
    void start();
    void stop();

// private:
    friend EchoServer& GetEchoServer(BDRequestCounter& c);

    BDRequestGenerator generator_;
    BDRequestHandler request_handler_;

    mtq::Queue<BDRequest> request_queue_;
    gen::ResourceManager<BDRequest> queue_manager;

    explicit EchoServer(BDRequestCounter& counter);

};

EchoServer& GetEchoServer(BDRequestCounter& c);

}