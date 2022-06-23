#ifndef __DBMANAGER__
#define __DBMANAGER__


#define QUERY_STR_MAX	1024

typedef int* cb(void* NotUsed, int argc, char** argv, char** azColName);

/********************************
 * DBManager(Singleton)
 ********************************/
class DBManager {

public:
	// Singleton should not be cloneable.
	DBManager(DBManager& other) = delete;

	//Singleton should not be assignable.
	void operator=(const DBManager&) = delete;

	//Singletone need to thread-safe
	static DBManager* getDBManager();
	void releaseDBManager();

	int createVehicleTable();
	int insertVehicle(int id, const char* plate, const char* info);

	int queryLogin(const char* id, const char* pwd, cb loginCallback);
	int queryLicensePlate(const char* plate, cb plateCallback);

private:
	sqlite3* db;
	int sessionCount;
	char queryStr[QUERY_STR_MAX];
	
	static DBManager* instance;

	//encapsulste for singleton
	DBManager();
	~DBManager();

	/*
	* Open DBMS
	* return: 0 = success, -1 = fail 
	*/
	int openDB(const char* dbNameB);

};

#endif

