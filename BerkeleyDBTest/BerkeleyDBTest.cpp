// loaddb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <db.h> 
#include <vector>

#define REQ_COUNT   10000

using namespace std;

int main()
{
    DB* dbp; 
    DBT key, data;
    char DBRecord[1024]{};
    u_int32_t flags; /* database open flags */
    int ret; /* function return value */
    clock_t t;

    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        printf("DB Create Error\n");
        return -1;
    }

    flags = DB_CREATE; /* If the database does not exist,
     * create it.*/
     /* open the database */
    ret = dbp->open(dbp, /* DB structure pointer */
        NULL, /* Transaction pointer */
        "licenseplate.db", /* On-disk file that holds the database. */
        NULL, /* Optional logical database name */
        DB_HASH, /* Database access method */
        flags, /* Open flags */
        0); /* File mode (using defaults) */
    if (ret != 0) {
        /* Error handling goes here */
        printf("DB Open Error\n");
        return -1;
    }

    const char plates[9][12] = { "LKY1360", "HHF6697", "GVP9164", "LBX9051", "06062", "LBV6157", "LVH6056", "ZPV5837", "ZDE1985" };

    fprintf(stderr, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    fprintf(stderr, "Request (%d-times) match result time in 150-records DB\n", REQ_COUNT);
    fprintf(stderr, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");


    for (int i = 0; i < 9; i++) {
        fprintf(stderr, ">>[%d] %s\t", i, plates[i]);

        t = clock();

        for (int j = 0; j < 10000; j++) {
            /* Zero out the DBTs before using them. */
            memset(&key, 0, sizeof(DBT));
            memset(&data, 0, sizeof(DBT));
            key.data = (char*)plates[i];
            key.size = (u_int32_t)(strlen(plates[i]) + 1);
            data.data = DBRecord;
            data.ulen = sizeof(DBRecord);
            data.flags = DB_DBT_USERMEM;
            if (dbp->get(dbp, NULL, &key, &data, 0) == DB_NOTFOUND) {
                fprintf(stderr, "\nfail to query !! [%d]\n", j);
            }
        }

        t = clock() - t;
        printf("\n Elapsed Time Milli-Sec :: %f \n", (float)t);

    }

    getchar();
    return 0;
}
