#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include "Exception.h"
#include "DBManager.h"
#include "UserEntry.h"	

class UserEntry;
class DBManager;
 
//---Server will create and use UsersManager
//--- ||    will be passing data in from client
class UsersManager : public DBManager {
public:
	//con/destructors
	UsersManager();
	~UsersManager();

	//checks
	bool userExists(const char *pName);
	bool login(UserEntry *pKey);
		//-!-return false if login failed

	//entries
	bool addNewUser(UserEntry *pEntry);
		//-!-return false if adding entry failed
	bool editExistingUser(UserEntry *pOldUser, UserEntry *pNewUser);
		//-!-return false if pOldEntry == pNewEntry i.e. edit failed
	bool removeUser(UserEntry *pUser);
		//-!-return false if user does not exist

	//retrieval
	UserEntry *getUsers(char *search, char *column);
	UserEntry getEntry(UserEntry *pKey);
};

#endif