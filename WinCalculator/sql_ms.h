#include <iostream>
#include "sqlite3.h"
#include <string>

using namespace std;

int logging_msgs(string user, string msg)
{
    int counter = 0;

    char* err;

    sqlite3* db;
    //sqlite3_stmt* stmt;

    sqlite3_open("message.sqlite", &db);

    string sql = "CREATE TABLE IF NOT EXISTS message_clients (id_client TEXT, msg TEXT PRIMARY KEY ,datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        cout << "sql:" << err << "\n";
    }
    else {
        string sqlstatement = "INSERT INTO CLIENTS (id,msg) VALUES ('" + user + "','" + msg + "'); ";

        rc = sqlite3_exec(db, sqlstatement.c_str(), NULL, NULL, &err);

    }




    sqlite3_close(db);


    return 0;

}