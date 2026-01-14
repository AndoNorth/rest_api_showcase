#pragma once

#include <mariadb/mysql.h>

class App
{
public:
    explicit App(MYSQL *db_conn);

    void run();  // blocks

private:
    MYSQL *db_conn;
};

