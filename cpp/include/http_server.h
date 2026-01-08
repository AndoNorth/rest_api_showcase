#pragma once

#include <iostream>
#include <string>

#include <SimpleJSON/json.hpp>
#include <served/multiplexer.hpp>
#include <served/net/server.hpp>

constexpr char kEndpoint[] = "/videos/{id:\\d+}";
constexpr char kIpAddress[] = "0.0.0.0";
constexpr char kPort[] = "5000";
constexpr int kThreads = 10;

class HttpServer
{
public:
    HttpServer(served::multiplexer multiplexer) : multiplexer(multiplexer) {}

    served::served_req_handler HandleGet()
    {
        return [&](served::response &response, const served::request &request)
        {
            std::cout << "GET request recieve for id:" << request.params["id"] << std::endl;
            json::JSON request_body = json::JSON::Load(request.body());
            /*
            json::JSON video_reponse = json::JSON::Load("get response");
            std::ostringstream stream;
            stream << video_reponse;
            response << stream.str();
            */
            bool get_successful = true;
            get_successful ? served::response::stock_reply(201, response)
                           : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandlePut()
    {
        return [&](served::response &response, const served::request &request)
        {
            std::cout << "PUT request recieve for id:" << request.params["id"] << std::endl;
            json::JSON request_body = json::JSON::Load(request.body());
            bool insert_successful = true;
            insert_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandlePost()
    {
        return [&](served::response &response, const served::request &request)
        {
            std::cout << "POST request recieve for id:" << request.params["id"] << std::endl;
            json::JSON request_body = json::JSON::Load(request.body());
            bool update_successful = true;
            update_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandleDelete()
    {
        return [&](served::response &response, const served::request &request)
        {
            std::cout << "DELETE request recieve for id:" << request.params["id"] << std::endl;
            json::JSON request_body = json::JSON::Load(request.body());
            bool delete_successful = true;
            delete_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    void InitialiseEndpoints()
    {
        served::methods_handler &handler = multiplexer.handle(kEndpoint);

        std::cout << "Registering GET handler" << std::endl;
        handler.get(HandleGet());
        std::cout << "Registering PUT handler" << std::endl;
        handler.put(HandlePut());
        std::cout << "Registering POST handler" << std::endl;
        handler.post(HandlePost());
        std::cout << "Registering DELETE handler" << std::endl;
        handler.del(HandleDelete());
    }

    void StartServer()
    {
        served::net::server server(kIpAddress, kPort, multiplexer);
        std::cout << "Starting server at \"" << kIpAddress
                  << "\" listening on port: " << kPort
                  << "..." << std::endl;
        server.run(kThreads);
    }

private:
    served::multiplexer multiplexer;
};
