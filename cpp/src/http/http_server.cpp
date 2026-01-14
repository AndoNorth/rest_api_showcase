#include <http/http_server.h>

HttpServer::HttpServer(served::multiplexer &multiplexer, MYSQL *db_conn)
    : multiplexer(multiplexer), db_conn(db_conn)
{
}

served::served_req_handler HttpServer::HandleGetAll()
{
    return [this](served::response &response, const served::request &request)
    {
        std::cout << "Handling Get All" << std::endl;
        std::ostringstream query;
        query << "SELECT * FROM videos";

        MYSQL_RES *res = mysql_execute_query(db_conn, query.str().c_str());

        if (!res)
        {
            response.set_status(500);
            response << "Database error";
            return;
        }

        MYSQL_ROW row;
        bool has_rows = false;
        json::JSON videos = json::JSON::Make(json::JSON::Class::Array);

        while ((row = mysql_fetch_row(res)) != NULL)
        {
            has_rows = true;
            json::JSON video;
            video["id"] = std::stoi(row[0]);
            video["name"] = row[1];
            video["likes"] = std::stoi(row[2]);
            video["views"] = std::stoi(row[3]);
            videos.append(video);
        }

        mysql_free_result(res);

        if (!has_rows)
        {
            response.set_status(404);
            response << "No videos found";
            return;
        }

        response.set_header("Content-Type", "application/json");
        response.set_status(200);
        response << videos.dump();
    };
}

served::served_req_handler HttpServer::HandleGet()
{
    return [this](served::response &response, const served::request &request)
    {
        int id = std::stoi(request.params["id"]);
        std::cout << "Handling Get with id:" << id << std::endl;
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

served::served_req_handler HttpServer::HandlePut()
{
    return [this](served::response &response, const served::request &request)
    {
        int id = std::stoi(request.params["id"]);
        std::cout << "Handling Put with id:" << id << std::endl;
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

served::served_req_handler HttpServer::HandlePost()
{
    return [this](served::response &response, const served::request &request)
    {
        int id = std::stoi(request.params["id"]);
        std::cout << "Handling Post with id:" << id << std::endl;
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

served::served_req_handler HttpServer::HandleDelete()
{
    return [this](served::response &response, const served::request &request)
    {
        int id = std::stoi(request.params["id"]);
        std::cout << "Handling Delete with id:" << id << std::endl;
        std::ostringstream query;
        query << "DELETE FROM videos WHERE id=" << id;

        mysql_execute_query(db_conn, query.str().c_str());

        response.set_status(200);
        response << "Video deleted";
    };
}

void HttpServer::InitialiseEndpoints()
{
    served::methods_handler &videos_handler = multiplexer.handle(kEndpointBase);
    videos_handler.get(HandleGetAll());

    served::methods_handler &videos_by_id_handler = multiplexer.handle(kEndpoint);
    videos_by_id_handler.get(HandleGet());
    videos_by_id_handler.put(HandlePut());
    videos_by_id_handler.post(HandlePost());
    videos_by_id_handler.del(HandleDelete());
}

void HttpServer::StartServer()
{
    served::net::server server(kIpAddress, kPort, multiplexer);
    std::cout << "Starting server at \"" << kIpAddress
              << "\" listening on port: " << kPort
              << "..." << std::endl;
    server.run(kThreads);
}

