#include <iostream>
#include "sqlite3.h"
#include <string>

using namespace std;

int logging_msgs(string user, string stg)
{
    int counter = 0;

    char* err;

    sqlite3* db;
    //sqlite3_stmt* stmt;

    sqlite3_open("message.sqlite", &db);

    string sql = "CREATE TABLE IF NOT EXISTS message (id TEXT, "\
        "stage TEXT,"\
        "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        cout << "ERROR (sql_E):" << rc << "\n";
    }
    else {
        string sqlstatement = "INSERT INTO message (id,stage) VALUES ('" + user + "','" + stg + "'); ";

        rc = sqlite3_exec(db, sqlstatement.c_str(), NULL, NULL, &err);

    }




    sqlite3_close(db);


    return 0;

}