#pragma comment(lib,"sqlite3.lib")
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string> 

#include "dbmanager.h"


#define DB_NAME         "licenseplate_sqlite3.db"

#define QUERY_CREATE_TABLE_VEHICLE  "CREATE TABLE VEHICLE(ID INT PRIMARY KEY NOT NULL, PLATE VARCHAR(20), INFO VARCHAR(1024));"
#define QUERY_INSERT_VEHICLE        "INSERT INTO VEHICLE(ID, PLATE, INFO) VALUES(%d, '%s', '%s');"

DBManager* DBManager::instance = nullptr;


DBManager* DBManager::getDBManager() {
    if (instance == nullptr) {
        instance = new DBManager();

        if (instance->openDB(DB_NAME) != 0) {
            delete instance;
            instance = nullptr;
        }
    }

    if (instance) {
        (instance->sessionCount)++;
    }
    return instance;
}

void DBManager::releaseDBManager()
{
    sessionCount--;

    if ((sessionCount <= 0) && (db != nullptr)) {
        sqlite3_close(db);
        db = nullptr;
    }
}

DBManager::DBManager() : db(nullptr), sessionCount(0) {
    memset(queryStr, 0, QUERY_STR_MAX);
}

DBManager::~DBManager() {}

int DBManager::queryLogin(const char* id, const char* pwd, cb loginCallback)
{
	return 0;
}

int DBManager::queryLicensePlate(const char* plate, cb plateCallback)
{
	return 0;
}

int DBManager::openDB(const char* dbNameB)
{
    if (db != nullptr) {
        fprintf(stderr, "DB already opened \n");
        return 0;
    }

    int rc = sqlite3_open(dbNameB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open DB: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    else {
        fprintf(stderr, "Success to open DB: %s\n", dbNameB);
    }

    // synchronous = OFF journal_mode=OFF TEMP_store = memory cash_SIZE + !))) PAGE_SIZE + @)$* COUNT_changes=OFF case_SENSITIVE_LIKE+Ffalse
    sqlite3_exec(db, "synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "journal_mode = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "temp_store = memory", NULL, NULL, NULL);
    sqlite3_exec(db, "cash_size = 1000", NULL, NULL, NULL);
    sqlite3_exec(db, "count_changes = OFF", NULL, NULL, NULL);
    sqlite3_exec(db, "case_sensitive_like = false", NULL, NULL, NULL);

	return 0;
}

int DBManager::createVehicleTable()
{
    char* zErrMsg = 0;

    if (db == nullptr) {
        fprintf(stderr, "DB is not opened\n");
        return -1;
    }

    int rc = sqlite3_exec(db, QUERY_CREATE_TABLE_VEHICLE, nullptr, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't create Table: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int DBManager::insertVehicle(int id, const char* plate, const char* info)
{
    char* zErrMsg = 0;

    if (db == nullptr) {
        fprintf(stderr, "DB is not opened\n");
        return -1;
    }

    sprintf_s(queryStr, QUERY_INSERT_VEHICLE, id, plate, info);

    int rc = sqlite3_exec(db, queryStr, nullptr, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't insert Vehicle: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}
