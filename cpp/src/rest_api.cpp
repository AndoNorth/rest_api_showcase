#include <iostream>

#include <http_server.h>

int main(int argc, char *argv[])
{
    // initialize connection to database
    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;

    struct connection_details mysqlDetails;
    mysqlDetails.server = "mysql";
    mysqlDetails.user = "rest_user";
    mysqlDetails.password = "password123";
    mysqlDetails.database = "rest_db_cpp";

    con = mysql_connection_setup(mysqlDetails);
    res = mysql_execute_query(con, "select * from videos");

    std::cout << "Displaying database output:"
              << std::endl;

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        std::cout << row[0] << " | "
                  << row[1] << " | "
                  << row[2] << " | "
                  << row[3]
                  << std::endl;
    }
    mysql_free_result(res);

    // create http server and initialize endpoints
    served::multiplexer multiplexer;
    HttpServer http_server(multiplexer, con);

    http_server.InitialiseEndpoints();
    http_server.StartServer(); // this is blocking, I think

    mysql_close(con);

    return 0;
}
