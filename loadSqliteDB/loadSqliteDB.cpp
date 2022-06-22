// loadsqlitedb.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#pragma comment(lib,"sqlite3.lib")
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string> 

#define DATA_FILE_NAME  "datafile.txt"
#define DB_NAME         "licenseplate_sqlite3.db"


//Async query callback
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;
}

int main()
{
    sqlite3* db;
    const char* sql;
    char* zErrMsg = 0;
    char insertSql[1024] = { 0, };
    int rc;
    std::ifstream file(DATA_FILE_NAME);
    std::string str;

    rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't create DB: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else {
        fprintf(stderr, "Success to create DB: %s\n", DB_NAME);
    }

    //Create Table
    sql = "CREATE TABLE VEHICLE(" \
        "ID     INT PRIMARY KEY NOT NULL," \
        "PLATE  VARCHAR(20)," \
        "INFO   VARCHAR(1024)" \
        "); ";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't create Table: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // synchronous = OFF journal_mode=OFF TEMP_store = memory cash_SIZE + !))) PAGE_SIZE + @)$* COUNT_changes=OFF case_SENSITIVE_LIKE+Ffalse
    sqlite3_exec(db, "synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "journal_mode = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "temp_store = memory", NULL, NULL, NULL);
    sqlite3_exec(db, "cash_size = 1000", NULL, NULL, NULL);
    sqlite3_exec(db, "count_changes = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "case_sensitive_like = false", NULL, NULL, NULL);

    //[byonghoon.jeon] Use default auto-commit transaction
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    int id = 0;
    while (std::getline(file, str, '$').good()) {
        std::string plate = str.substr(0, str.find("\n"));
        printf("[%d] plate is %s\n", id, plate.c_str());

        sprintf_s(insertSql,
            "INSERT INTO VEHICLE (ID, PLATE, INFO) VALUES (%d, '%s', '%s' ); ",
            id, plate.c_str(), str.c_str());

        rc = sqlite3_exec(db, insertSql, callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Can't insert records: %s\n", sqlite3_errmsg(db));
            return 0;
        }

        id++;
    }

    rc = sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_close(db);
    return 0;
}