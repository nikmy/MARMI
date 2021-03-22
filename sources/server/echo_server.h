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
    void restart();
    void shutdown();

    int64_t get_backup_size();

    friend EchoServer& GetEchoServer(BDRequestCounter& c);

 private:
    BDRequestGenerator generator_;
    BDRequestHandler request_handler_;
    gen::ResourceManager<BDRequest> requests_manager_;

    gen::Queue<BDRequest> backup_;

    explicit EchoServer(BDRequestCounter& counter);
};

EchoServer& GetEchoServer(BDRequestCounter& c);

}