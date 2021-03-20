#include "echo_server.h"

namespace server
{

EchoServer::EchoServer(BDRequestCounter& counter)
    : generator_(counter),
      request_handler_(counter),
      request_queue_(32),
      queue_manager(
          request_queue_,
          [&]() {
              BDRequest x = generator_.getRequest();
              request_queue_.push(x);
          },
          [&](BDRequest r) { request_handler_.process(std::move(r)); },
          [&]() { },
          [&]() { },
          7,
          1
      )
{ }

void EchoServer::start()
{ queue_manager.start(); }

void EchoServer::stop()
{ queue_manager.stop(); }

EchoServer& GetEchoServer(BDRequestCounter& c)
{
    static EchoServer server(c);
    return server;
}

}
