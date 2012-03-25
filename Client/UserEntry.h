#ifndef USERENTRY_H
#define USERENTRY_H

#define SIZE 64 //size of stuff, normal entry

#include <stdlib.h>
#include <string>

/*UserKey
	this is to test a user against a databse for login
	this is sepearte, due to functionality. in later projects
	i will have the key encrypted, and decrypted etc.
*/ 
class UserKey { 
protected:
	char mUserName[SIZE];
	char mUserPwd[SIZE];
public:
	//con/destructor
	UserKey();
	UserKey(char *pUserName, char *pUserPwd);
	//setUp 
	void setup(char *pUserName, char *pUserPwd);
	//retrieval
	char **getKey();
	const char *getUserName();
	const char *getUserPwd();
};

//this class is meant to be a quick place holder for values
//and is to be used to make code look neater when passing
//values too and from the databse

//it is also meant to be an easy way to return data needed
//to complete a successful sql query regarding
//user entries and logins
class UserEntry {
private:
	char mOtherInfo[SIZE]; //this is generic just to be here, might not use it.
						        //more or less to show the use of a seperate UserKey class
	UserKey key;
public:
	//con/destructors
	//3 ways to initialize
	//1. blank (to be defined) 
	UserEntry();
	//2. defined as new entry
	UserEntry(char *pUserName, char *pUserPwd, char *ohterInfo);
	//3. defined as key
	UserEntry(UserKey *pKey);
	~UserEntry();

	//setup
	void setup(char *pUserName, char *pUserPwd, char *ohterInfo);
	void setupKey(UserKey *pKey);

	//retrieval 
	const char *getOtherInfo();
	const char *getKeyQuery();
	const char *getNewUserQuery();
	UserKey getKey();
};

#endif