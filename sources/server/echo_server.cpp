#include "echo_server.h"

namespace server
{

EchoServer::EchoServer(BDRequestCounter& counter)
    : generator_(counter),
      request_handler_(counter),
      requests_manager_(
          generator_,
          request_handler_,
          1024,
          16
      ),
      backup_(1024)
{ }

void EchoServer::start()
{ requests_manager_.start(); }

void EchoServer::stop()
{ requests_manager_.stop(); }

void EchoServer::restart()
{
    requests_manager_.save_session_data(backup_);
    requests_manager_.restore_session_data(backup_);

    start();
}

void EchoServer::shutdown()
{
    requests_manager_.stop();
    requests_manager_.save_session_data(backup_);
}

EchoServer& GetEchoServer(BDRequestCounter& c)
{
    static EchoServer server(c);
    return server;
}

int64_t EchoServer::get_backup_size()
{ return backup_.size(); }

}
