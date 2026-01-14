#pragma once

#include <mariadb/mysql.h>

struct connection_details
{
    const char *server;
    const char *user;
    const char *password;
    const char *database;
};

MYSQL *mysql_connection_setup(const connection_details &details);
MYSQL_RES *mysql_execute_query(MYSQL *connection, const char *sql_query);
