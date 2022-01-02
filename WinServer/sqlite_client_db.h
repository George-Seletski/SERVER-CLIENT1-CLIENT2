#include <iostream>
#include "sqlite3.h"
#include <string>

const char* DB_FILE_NAME_C = "clients.sqlite";

sqlite3* db_c = nullptr;
sqlite3_stmt* ins_log_stmt_c = nullptr;
//int rc;

int create_tables_c() {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(
        db_c,
        "CREATE TABLE IF NOT EXISTS clients("\
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "c_name TEXT"\
        ")",
        NULL,
        0,
        &err_msg
    );
    if (rc) {
        std::cout << "Error(C DB): " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
    return rc;
}

int prepare_statements_c() {
    std::string sql = "INSERT INTO clients (c_name) VALUES (?);";
    return sqlite3_prepare_v2(
        db_c,
        sql.c_str(),
        sql.length(),
        &ins_log_stmt_c,
        nullptr
    );
}

static int logs_callback_c(void* data, int argc, char** argv, char** azColName) {
    if (argc == 3) {
        std::cout << argv[0] << ". (" << argv[1] << ") " << std::endl;
    }
    else {
        std::cout << "Error: Expected 2 columns, got " << argc << std::endl;
    }

    return 0;
}

int list_logs_c() {
    char* err_msg = nullptr;
    std::string sql = "SELECT id, c_name FROM clients";
    int rc = sqlite3_exec(db_c, sql.c_str(), logs_callback_c, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        std::cout << "Error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
    return rc;
}

int insert_log_c(std::string data) {
    int rc = sqlite3_bind_text(
        ins_log_stmt_c,
        1,
        data.c_str(),
        data.length(),
        SQLITE_STATIC
    );
    if (rc) {
        return rc;
    }

    rc = sqlite3_step(ins_log_stmt_c);
    if (rc != SQLITE_DONE) {
        return rc;
    }

    rc = sqlite3_reset(ins_log_stmt_c);
    if (rc) {
        return rc;
    }

    return rc;
}