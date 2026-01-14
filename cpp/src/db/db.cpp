#include <db/db.h>
#include <iostream>
#include <cstdlib>

MYSQL *mysql_connection_setup(const connection_details &details)
{
    MYSQL *connection = mysql_init(nullptr);

    if (!mysql_real_connect(
            connection,
            details.server,
            details.user,
            details.password,
            details.database,
            0,
            nullptr,
            0))
    {
        std::cerr << "MySQL connection error: "
                  << mysql_error(connection) << std::endl;
        std::exit(1);
    }

    std::cout << "Connected to MySQL" << std::endl;
    return connection;
}

MYSQL_RES *mysql_execute_query(MYSQL *connection, const char *sql_query)
{
    if (mysql_query(connection, sql_query))
    {
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return mysql_use_result(connection);
}
