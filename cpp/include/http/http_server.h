#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include <SimpleJSON/json.hpp>
#include <served/multiplexer.hpp>
#include <served/net/server.hpp>
#include <db/db.h>

constexpr char kEndpointBase[] = "/videos";
constexpr char kEndpoint[] = "/video/{id:\\d+}";
constexpr char kIpAddress[] = "0.0.0.0";
constexpr char kPort[] = "5000";
constexpr int kThreads = 10;

class HttpServer
{
public:
    HttpServer(served::multiplexer &multiplexer, MYSQL *db_conn);

    void InitialiseEndpoints();
    void StartServer();

private:
    // Handlers
    served::served_req_handler HandleGetAll();
    served::served_req_handler HandleGet();
    served::served_req_handler HandlePut();
    served::served_req_handler HandlePost();
    served::served_req_handler HandleDelete();

private:
    served::multiplexer &multiplexer;
    MYSQL *db_conn;
};

