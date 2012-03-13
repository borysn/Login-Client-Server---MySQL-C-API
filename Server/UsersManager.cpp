#include "UsersManager.h"

//con/destructors
UsersManager::UsersManager() : DBManager() {
	strcpy_s(info->host, SIZE, "localhost");
	strcpy_s(info->user, SIZE, "");
	strcpy_s(info->pass, SIZE, "");
	strcpy_s(info->db, SIZE, "login");
	strcpy_s(info->tables[0], SIZE, "users"); 

	//i dont think this is necessary any more 
	//setConnectionInfo(info); 
	if(!setupConnection()) {
		printf_s("\nerror:: %s\n\n", mysql_error(m_pConn));
		exit(1);
	}
} 

UsersManager::~UsersManager() {
}

//checks
bool UsersManager::userExists(const char *pName) { 
	char *query = new char[SIZE*2];

	//see if user is in database query, more cat games
	strcpy_s(query, SIZE*2, "SELECT * FROM ");
	strcat_s(query, SIZE*2, info->tables[0]);
	strcat_s(query, SIZE*2, " WHERE username = '"); 
	strcat_s(query, SIZE*2, pName);
	strcat_s(query, SIZE*2, "'");
	cout << "\n-----------\nquery = " << query << "\n-----------\n";
	cout << "\nSeeing if user exists...\n";

	doQuery(query);

	try { 
		char **tmp; 
		if ((tmp = getNextRow()) != NULL) { 
			cout << "\ntmp[0] = " << tmp[0] << endl;
			if (strcmp(tmp[0], pName) == 0) {
				cout << "\tMatch found!\n\tUser: " << tmp[0] << endl; 
			} else
				throw Exception("\tError, no match found...");
		} else
			throw Exception("\tSearch came up empty.\n");
	} catch (Exception &e) {
		cout << e.hmm();
		delete [] query;
		return false;
	}

	delete [] query;
	return true;
}

bool UsersManager::login(UserEntry *pKey) {
	//-!-return false if login failed  
	return true;
}


//entries
bool UsersManager::addNewUser(UserEntry *pEntry) {
	char *query = new char[SIZE*2];

	//setup query to add new user
	strcpy_s(query, SIZE*2, "INSERT INTO ");
	strcat_s(query, SIZE*2, info->tables[0]);
	strcat_s(query, SIZE*2, pEntry->getNewUserQuery()); 

	try {
		//dont add user if the name exists already
		if (userExists(pEntry->getKey().getUserName()))
			throw Exception("\n\tUser already exists...\n");
		cout << "\nUser does not exist, running query...\n";
		cout << "\n-----------\nquery = " << query << "\n-----------\n";

		doQuery(query);	

		cout << "\nQuery complete, testing database for entry...\n";

		if (userExists(pEntry->getKey().getUserName()) == false)
			throw Exception("\n\tFailed to add new user...\n");
	} catch (Exception &e) {
		cout << e.hmm();
		delete [] query;
		return false;
	}

	delete [] query;
	return true;
}

bool UsersManager::editExistingUser(UserEntry *pOldUser, UserEntry *pNewUser) {
	//-!-return false if pOldEntry == pNewEntry i.e. edit failed  
	return true;
}

bool UsersManager::removeUser(UserEntry *pUser) {
	//-!-return false if user does not exist 
	return true;
}


//retrieval
UserEntry *UsersManager::getUsers(char *search, char *column) {
	return new UserEntry();
}

UserEntry UsersManager::getEntry(UserEntry *pKey) {
	return UserEntry();
}
