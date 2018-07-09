//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#include "db_server_service.hpp"
#include "db_server.hpp"
#include "db_storage_service.hpp"

#include "wsrep/logger.hpp"
#include "wsrep/high_priority_service.hpp"

db::server_service::server_service(db::server& server)
    : server_(server)
{ }

wsrep::storage_service* db::server_service::storage_service(
    wsrep::client_service&)
{
    return new db::storage_service();
}

void db::server_service::release_storage_service(
    wsrep::storage_service* storage_service)
{
    delete storage_service;
}

wsrep::high_priority_service* db::server_service::streaming_applier_service(
    wsrep::client_service&)
{
    return server_.streaming_applier_service();
}

wsrep::high_priority_service* db::server_service::streaming_applier_service(
    wsrep::high_priority_service&)
{
    return server_.streaming_applier_service();
}

void db::server_service::release_high_priority_service(
    wsrep::high_priority_service *high_priority_service)
{
    delete high_priority_service;
}

bool db::server_service::sst_before_init() const
{
    return true;
}

std::string db::server_service::sst_request()
{
    std::ostringstream os;
    os << server_.server_state().id();
    wsrep::log_info() << "SST request: "
                      << server_.server_state().id();

    return os.str();
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

void db::server_service::bootstrap()
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

void db::server_service::log_state_change(
    enum wsrep::server_state::state prev_state,
    enum wsrep::server_state::state current_state)
{

    wsrep::log_info() << "State changed "
                      << prev_state << " -> " << current_state;
}
int db::server_service::wait_committing_transactions(int)
{
    throw wsrep::not_implemented_error();
}

void db::server_service::debug_sync(const char*)
{

}
