#pragma once

#include <iostream>
#include <string>

#include <SimpleJSON/json.hpp>
#include <served/multiplexer.hpp>
#include <served/net/server.hpp>
#include <sql_driver.h>


constexpr char kEndpoint[] = "/videos/{id:\\d+}";
constexpr char kIpAddress[] = "0.0.0.0";
constexpr char kPort[] = "5000";
constexpr int kThreads = 10;

class HttpServer
{
public:
    HttpServer(served::multiplexer multiplexer, MYSQL *db_conn)
        : multiplexer(multiplexer), db_conn(db_conn) {}

    served::served_req_handler HandleGet()
    {
        return [this](served::response &response, const served::request &request)
        {
            int id = std::stoi(request.params["id"]);
            std::ostringstream query;
            query << "SELECT * FROM videos WHERE id=" << id;

            MYSQL_RES *res = mysql_execute_query(db_conn, query.str().c_str());
            MYSQL_ROW row = mysql_fetch_row(res);

            if (row != NULL)
            {
                json::JSON video;
                video["id"] = std::stoi(row[0]);
                video["name"] = row[1];
                video["likes"] = std::stoi(row[2]);
                video["views"] = std::stoi(row[3]);

                std::ostringstream stream;
                stream << video;
                response << stream.str();
                response.set_header("Content-Type", "application/json");
                response.set_status(200);
            }
            else
            {
                response.set_status(404);
                response << "Video not found";
            }

            mysql_free_result(res);
        };
    }

    served::served_req_handler HandlePut()
    {
        return [this](served::response &response, const served::request &request)
        {
            int id = std::stoi(request.params["id"]);
            json::JSON body = json::JSON::Load(request.body());

            std::ostringstream query;
            query << "REPLACE INTO videos (id, name, likes, views) VALUES ("
                  << id << ", '"
                  << body["name"].ToString() << "', "
                  << body["likes"].ToInt() << ", "
                  << body["views"].ToInt() << ")";

            mysql_execute_query(db_conn, query.str().c_str());

            response.set_status(200);
            response << "Video created/updated";
        };
    }

    served::served_req_handler HandlePost()
    {
        return [this](served::response &response, const served::request &request)
        {
            int id = std::stoi(request.params["id"]);
            json::JSON body = json::JSON::Load(request.body());

            std::ostringstream query;
            query << "UPDATE videos SET ";
            bool first = true;

            if (body.hasKey("name"))
            {
                query << "name='" << body["name"].ToString() << "'";
                first = false;
            }
            if (body.hasKey("likes"))
            {
                if (!first) query << ", ";
                query << "likes=" << body["likes"].ToInt();
                first = false;
            }
            if (body.hasKey("views"))
            {
                if (!first) query << ", ";
                query << "views=" << body["views"].ToInt();
            }

            query << " WHERE id=" << id;

            mysql_execute_query(db_conn, query.str().c_str());

            response.set_status(200);
            response << "Video updated";
        };
    }

    served::served_req_handler HandleDelete()
    {
        return [this](served::response &response, const served::request &request)
        {
            int id = std::stoi(request.params["id"]);

            std::ostringstream query;
            query << "DELETE FROM videos WHERE id=" << id;

            mysql_execute_query(db_conn, query.str().c_str());

            response.set_status(200);
            response << "Video deleted";
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
    served::multiplexer &multiplexer;
    MYSQL *db_conn;
};
