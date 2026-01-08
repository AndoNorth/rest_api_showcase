#include <iostream>
// sudo apt-get install libmariadb3 libmariadb-dev
#include <mariadb/mysql.h>

// database using mariadb
struct connection_details
{
    const char *server, *user, *password, *database;
};

MYSQL *mysql_connection_setup(struct connection_details mysql_details)
{
    MYSQL *connection = mysql_init(NULL);

    if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user,
                            mysql_details.password, mysql_details.database,
                            0, NULL, 0))
    {
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
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
