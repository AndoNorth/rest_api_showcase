#include <app/app.h>
#include <http/http_server.h>

App::App(MYSQL *db_conn) : db_conn(db_conn) {}

void App::run()
{
    served::multiplexer multiplexer;
    HttpServer http_server(multiplexer, db_conn);
    http_server.InitialiseEndpoints();

    std::cout << "Starting HTTP server..." << std::endl;
    http_server.StartServer();  // BLOCKING
}

