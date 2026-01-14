#include <iostream>

#include <app/app.h>
#include <db/db.h>

int main(int argc, char *argv[])
{
    connection_details mysqlDetails{
        .server   = "mysql",
        .user     = "rest_user",
        .password = "password123",
        .database = "rest_db_cpp",
    };

    MYSQL *db = mysql_connection_setup(mysqlDetails);

    App app(db);
    app.run();   // blocks forever

    // unreachable unless server stops
    mysql_close(db);

    return 0;
}

