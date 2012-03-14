#include "DBManager.h"

DBManager::DBManager() {
	info = new MysqlConnectInfo(); 
	m_pConn = mysql_init(NULL); 
	m_pResult = NULL; 
	strcpy_s(info->host, SIZE, "\0");
	strcpy_s(info->user, SIZE, "\0"); 
	strcpy_s(info->pass, SIZE, "\0"); 
	strcpy_s(info->db, SIZE, "\0"); 
	strcpy_s(info->tables[1], SIZE, "\0"); 
} 

DBManager::DBManager(MysqlConnectInfo *pDBInfo) {
	setConnectionInfo(pDBInfo);
	strcpy_s(info->db, SIZE, pDBInfo->db);
	strcpy_s(info->host, SIZE, pDBInfo->host);
	strcpy_s(info->pass, SIZE, pDBInfo->pass);
	strcpy_s(info->tables[1], SIZE, pDBInfo->tables[1]);
	strcpy_s(info->user, SIZE, pDBInfo->user);

	m_pConn = mysql_init(NULL);
	m_pResult = NULL;
}

DBManager::~DBManager() {
	//clean up
	mysql_free_result(m_pResult);
	mysql_close(m_pConn);
	delete m_pResult;
	delete m_pConn;
	delete info;
}

bool DBManager::DBExists(){
	//first setup query to search for specific db
	char query[SIZE] = "SHOW DATABASES LIKE '";
	strcat_s(query, SIZE, info->db);
	strcat_s(query, SIZE, "'");
	//run query amd save pResult
	doQuery(query);
	char tmp[SIZE];
	strcpy_s(tmp, SIZE, getNextRow()[0]);
	//if query returned pResult return true, 
	//else return false
	if (strcmp(tmp, info->db) == 0) 	
		return true;
	return false;
}

bool DBManager::DBTableExists(char *pTable){
	//first setup query to search for specific
	char query[SIZE] = "SHOW TABLES FROM ";
	strcat_s(query, SIZE, info->db);
	strcat_s(query, SIZE, " LIKE '");
	strcat_s(query, SIZE, pTable); 
	strcat_s(query, SIZE, "'"); 
	//run query amd save pResult
	doQuery(query);
	char tmp[SIZE];
	strcpy_s(tmp, SIZE, getNextRow()[0]);
	if (strcmp(tmp, pTable) == 0)
		return true;
	return false; 
}

bool DBManager::setupConnection() {
	if (!mysql_real_connect(m_pConn, info->host, info->user, info->pass, 
		                    info->db, 0, NULL, 0))
		return false;
	else 
		return true;
}

void DBManager::setConnectionInfo(MysqlConnectInfo *pdbInfo) {
	info = pdbInfo;
} 

void DBManager::doQuery(char *pQuery) {
	mysql_free_result(m_pResult);
	if (mysql_query(m_pConn, pQuery)) { 
		m_pResult = NULL;	
		exit(1); 
	}
	else { 
		m_pResult = mysql_store_result(m_pConn);
	}
}

char **DBManager::getNextRow() {
	if (!(m_pResult == NULL))
		return mysql_fetch_row(m_pResult);
	else
		return NULL;
} 

MYSQL_RES *DBManager::getResult() {
	return m_pResult;
}

MYSQL *DBManager::getConnection() {
	return m_pConn;
}

