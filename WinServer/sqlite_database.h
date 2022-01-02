#include <iostream>
#include "sqlite3.h"
#include <string>

const char* DB_FILE_NAME = "logs.sqlite";

sqlite3* db = nullptr;
sqlite3_stmt* ins_log_stmt = nullptr;
//int rc;

int create_tables() {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS logs("\
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"\
        "data TEXT"\
        ")",
        NULL,
        0,
        &err_msg
    );
    if (rc) {
        std::cout << "Error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
    return rc;
}

int prepare_statements() {
    std::string sql = "INSERT INTO logs (data) VALUES (?);";
    return sqlite3_prepare_v2(
        db,
        sql.c_str(),
        sql.length(),
        &ins_log_stmt,
        nullptr
    );
}

static int logs_callback(void* data, int argc, char** argv, char** azColName) {
    if (argc == 3) {
        std::cout << argv[0] << ". (" << argv[2] << ") " << argv[1] << std::endl;
    }
    else {
        std::cout << "Error: Expected 3 columns, got " << argc << std::endl;
    }

    return 0;
}

int list_logs() {
    char* err_msg = nullptr;
    std::string sql = "SELECT id, datetime(datetime,'localtime'), data FROM logs";
    int rc = sqlite3_exec(db, sql.c_str(), logs_callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        std::cout << "Error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
    return rc;
}

int insert_log(std::string data) {
    int rc = sqlite3_bind_text(
        ins_log_stmt,
        1,
        data.c_str(),
        data.length(),
        SQLITE_STATIC
    );
    if (rc) {
        return rc;
    }

    rc = sqlite3_step(ins_log_stmt);
    if (rc != SQLITE_DONE) {
        return rc;
    }

    rc = sqlite3_reset(ins_log_stmt);
    if (rc) {
        return rc;
    }

    return rc;
}