// loadsqlitedb.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib,"sqlite3.lib")
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string> 

#include "dbmanager.h"

#define DATA_FILE_NAME  "datafile.txt"



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
    int rc;
    std::ifstream file(DATA_FILE_NAME);
    std::string str;

    DBManager* dbms = DBManager::getDBManager();
    if (dbms == nullptr) {
        fprintf(stderr, "Fail to get DBManager\n");
        return 0;
    }

    if (dbms->createVehicleTable() != 0) {
        fprintf(stderr, "Fail to create Vehicle Table\n");
        return 0;
    }

    int id = 0;
    while (std::getline(file, str, '$').good()) {
        std::string plate = str.substr(0, str.find("\n"));
        printf("[%d] plate is %s\n", id, plate.c_str());

        rc = dbms->insertVehicle(id, plate.c_str(), str.c_str());
        if (rc != 0) {
            fprintf(stderr, "Fail to insert vehicle record\n");
            return 0;
        }

        id++;
    }

    dbms->releaseDBManager();
    return 0;
}