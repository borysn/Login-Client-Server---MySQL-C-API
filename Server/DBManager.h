#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <iostream> 
#include <string.h>
#include <WinSock.h>
#include <mysql.h>

using namespace std;

#define SIZE 64

/* this struct will be a quick place holder for
   mysql connection info
*/
struct MysqlConnectInfo {
	char host[SIZE];
	char user[SIZE]; 
	char pass[SIZE];
	char db[SIZE];
	char tables[1][SIZE]; //only one table so far
};

//this class will never be used by itself
//it will always be the base of another class
class DBManager {
protected:
	MysqlConnectInfo *info;
	MYSQL *m_pConn;
	MYSQL_RES *m_pResult; 
public: 
	//[con/destructor]
	DBManager();
	DBManager(MysqlConnectInfo *pDBInfo);
	~DBManager();
	
	//checks
	bool DBExists();
	bool DBTableExists(char *pTable);

	/*/define later*/
	bool createDB(); 
	bool clearDB();  
	
	bool setupConnection(); 
	//this function will setup the pConnection then return a pointer to it
	void setConnectionInfo(MysqlConnectInfo *pDBInfo); 
	//this function will set the pConnect info
	void doQuery(char *pQuery); 
	//this function will perform an sql query, and return a pointer to the pResult
	// sets pResult to NULL if query is not succesful
	char **getNextRow(); 
	//returns pResult as row (char**)
	//return null if pResult is empty 
	MYSQL_RES *getResult();
	MYSQL *getConnection();
	//return pConn
};

#endif