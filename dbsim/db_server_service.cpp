//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#include "db_server_service.hpp"
#include "db_server.hpp"

#include "wsrep/logger.hpp"

db::server_service::server_service(db::server& server)
    : server_(server)
{ }

wsrep::client_state* db::server_service::local_client_state()
{
    return server_.local_client_state();
}

wsrep::client_state* db::server_service::streaming_applier_client_state()
{
    return server_.streaming_applier_client_state();
}

void db::server_service::release_client_state(
    wsrep::client_state* client_state)
{
    server_.release_client_state(client_state);
}

bool db::server_service::sst_before_init() const
{
    return true;
}

std::string db::server_service::sst_request()
{
    return server_.server_state().id();
}

int db::server_service::start_sst(
    const std::string& request, const wsrep::gtid& gtid, bool bypass)
{
    server_.donate_sst(request, gtid, bypass);
    return 0;
}

void db::server_service::background_rollback(wsrep::client_state&)
{
}

void db::server_service::log_message(enum wsrep::log::level level,
                                     const char* message)
{
    wsrep::log(level, server_.server_state().name().c_str()) << message;
}
void db::server_service::log_dummy_write_set(
    wsrep::client_state&, const wsrep::ws_meta& meta)
{
    wsrep::log_info() << "Dummy write set: " << meta.seqno();
}

void db::server_service::log_view(const wsrep::view&)
{
    wsrep::log_info() << "View";
}