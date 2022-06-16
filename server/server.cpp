// server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>
#include "NetworkTCP.h"
#include <Windows.h>
#include <db.h> 
#if defined(_USE_MQTT_)
#include "mqtt.h"
#include <thread>

mqtt _mqtt{};
#endif

DB* dbp; /* DB structure handle */

bool lookupDB(char* PlateString, DBT& data)
{
  char DBRecord[2048]{};
  DBT key;
  
  /* Zero out the DBTs before using them. */
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  key.data = PlateString;
  key.size = (u_int32_t)(strlen(PlateString) + 1);
  data.data = DBRecord;
  data.ulen = sizeof(DBRecord);
  data.flags = DB_DBT_USERMEM;
  return dbp->get(dbp, NULL, &key, &data, 0) != DB_NOTFOUND;
}

void processPlateQuery(void* msgBuf, int msgLen)
{
  char* PlateString = reinterpret_cast<char*>(msgBuf);
  printf("Plate is : %s\n", PlateString);

  if (DBT data; lookupDB(PlateString, data))
  {
    int sendlength = (int)(strlen((char*)data.data) + 1);
    _mqtt.publish("ALPR_TOPIC", (unsigned char*)data.data, sendlength, 0);
    printf("sent ->%s\n", (char*)data.data);
  }
}

int main()
{
#if !defined(_USE_MQTT_)
    TTcpListenPort* TcpListenPort;
    TTcpConnectedPort* TcpConnectedPort;
    struct sockaddr_in cli_addr;
    socklen_t          clilen;
#endif
    bool NeedStringLength = true;
    unsigned short PlateStringLength;
    char PlateString[1024];
    
    u_int32_t flags; /* database open flags */
    int ret; /* function return value */
    ssize_t result;

    /* Initialize the structure. This
     * database is not opened in an environment,
     * so the environment pointer is NULL. */
    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        /* Error handling goes here */
        printf("DB Create Error\n");
        return -1;
    }
    /* Database open flags */
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

#if defined(_USE_MQTT_)
    _mqtt.secure(false, "./rootca.crt", nullptr, "./client.crt", "./client.key");
    _mqtt.connect("localhost", 60);

    _mqtt.regTopic("ALPR_TOPIC", std::bind(processPlateQuery, std::placeholders::_1, std::placeholders::_2));
    _mqtt.subscribeTopic("ALPR_TOPIC", 0);

    while (1)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#else
    std::cout << "Listening\n";
    if ((TcpListenPort = OpenTcpListenPort(2222)) == NULL)  // Open UDP Network port
    {
        std::cout << "OpenTcpListenPortFailed\n";
        return(-1);
    }
    clilen = sizeof(cli_addr);
    if ((TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen)) == NULL)
    {
        printf("AcceptTcpConnection Failed\n");
        return(-1);
    }
    printf("connected\n");
    while (1)
    {
        if (ReadDataTcp(TcpConnectedPort, (unsigned char*)&PlateStringLength, sizeof(PlateStringLength)) != sizeof(PlateStringLength))
        {
            printf("ReadDataTcp 1 error\n");
            return(-1);
        }
        PlateStringLength = ntohs(PlateStringLength);
        if (PlateStringLength > sizeof(PlateString))
        {
            printf("Plate string length  error\n");
            return(-1);
        }
        if (ReadDataTcp(TcpConnectedPort, (unsigned char*)&PlateString, PlateStringLength) != PlateStringLength)
        {
            printf("ReadDataTcp 2 error\n");
            return(-1);
        }
        printf("Plate is : %s\n", PlateString);

        /* Zero out the DBTs before using them. */
        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        key.data = PlateString;
        key.size = (u_int32_t) (strlen(PlateString)+1);
        data.data = DBRecord;
        data.ulen = sizeof(DBRecord);
        data.flags = DB_DBT_USERMEM;
        if (dbp->get(dbp, NULL, &key, &data, 0) != DB_NOTFOUND)
        {
            int sendlength = (int)(strlen((char*)data.data) + 1);
            short SendMsgHdr=ntohs(sendlength);
            if ((result = WriteDataTcp(TcpConnectedPort, (unsigned char*)&SendMsgHdr, sizeof(SendMsgHdr))) != sizeof(SendMsgHdr))
                printf("WriteDataTcp %lld\n", result);
            if ((result = WriteDataTcp(TcpConnectedPort, (unsigned char*)data.data, sendlength)) != sendlength)
                printf("WriteDataTcp %lld\n", result);
            printf("sent ->%s\n", (char*)data.data);
        }
       // else printf("not Found\n");



    }
#endif
}



