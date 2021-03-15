#include "echo_server.h"

namespace server
{

EchoServer::EchoServer(BDRequestCounter& counter)
    : generator_(counter),
      request_handler_(counter),
      request_queue_(32),
      queue_manager(
          request_queue_,
          [&]() { BDRequest x = generator_.getRequest(); request_queue_.push(x); },
          [&]() { return request_queue_.take_first(); },
          [&](BDRequest r) { request_handler_.process(r); },
          [&]() { return request_queue_.empty(); },
          [&]() { return request_queue_.size() == request_queue_.max_size(); },
          8
          )
{ }

void EchoServer::start()
{ queue_manager.start(); }

void EchoServer::stop()
{ queue_manager.stop(); }

EchoServer& GetEchoServer (BDRequestCounter& c) {
    static EchoServer server(c);
    return server;
}

}
