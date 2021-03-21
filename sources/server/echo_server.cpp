#include "echo_server.h"

namespace server
{

EchoServer::EchoServer(BDRequestCounter& counter)
    : generator_(counter),
      request_handler_(counter),
      requests_manager_(
          generator_,
          request_handler_,
          64,
          8
      )
{ }

void EchoServer::start()
{ requests_manager_.start(); }

void EchoServer::stop()
{ requests_manager_.stop(); }

void EchoServer::shutdown()
{ requests_manager_.shutdown(); }

EchoServer& GetEchoServer(BDRequestCounter& c)
{
    static EchoServer server(c);
    return server;
}

}
