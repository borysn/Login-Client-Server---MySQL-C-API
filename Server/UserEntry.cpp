#include "UserEntry.h"

//------UserKey class-------
UserKey::UserKey() {
	strcpy_s(mUserName, SIZE, "\0");
	strcpy_s(mUserPwd, SIZE, "\0");
}

UserKey::UserKey(char *pUserName, char *pUserPwd) {
	strcpy_s(mUserName, SIZE, pUserName);
	strcpy_s(mUserPwd, SIZE, pUserPwd);
}

//setUp 
void UserKey::setup(char *pUserName, char *pUserPwd) {
	strcpy_s(mUserName, SIZE, pUserName);
	strcpy_s(mUserPwd, SIZE, pUserPwd);
}

//retrieval
char **UserKey::getKey() {
	char **tmp = new char*[SIZE];
	strcpy_s(tmp[0], SIZE, getUserName());
	strcpy_s(tmp[1], SIZE, getUserPwd());
	return tmp;
}

const char *UserKey::getUserName() {
	return mUserName;
}

const char *UserKey::getUserPwd() {
	return mUserPwd;
}

//------UserEntry class-------
//con/destructors
//3 ways to initialize
//1. blank (to be defined)
UserEntry::UserEntry() {
	strcpy_s(mOtherInfo, SIZE, "\0");
	key = UserKey();
}

//2. defined as new entry
UserEntry::UserEntry(char *pUserName, char *pUserPwd, char *pOtherInfo) {
	//setup
	setup(pUserName, pUserPwd, pOtherInfo);
}

//3. defined as key
UserEntry::UserEntry(UserKey *pKey) {
	//other info is null
	strcpy_s(mOtherInfo, SIZE, "\0");
	//create key from pKey
	setupKey(pKey);
}

UserEntry::~UserEntry() {
	//nothing really to do here
}

//setup
void UserEntry::setup(char *pUserName, char *pUserPwd, char *pOtherInfo) {
	strcpy_s(mOtherInfo, SIZE, pOtherInfo);
	key.setup(pUserName, pUserPwd); 
}

void UserEntry::setupKey(UserKey *pKey) {
	key = UserKey(*pKey);
}

//retrieval 
const char *UserEntry::getOtherInfo() {
	return mOtherInfo;
}

const char *UserEntry::getKeyQuery() {
	char *query = new char[SIZE];

	//setup query, cat game
	strcpy_s(query, SIZE, " username = '");
	strcat_s(query, SIZE, key.getUserName());
	strcat_s(query, SIZE, "' AND password = '");
	strcat_s(query, SIZE, key.getUserPwd());
	strcat_s(query, SIZE, "'");

	return query;
}

const char *UserEntry::getNewUserQuery() {
	char *query = new char[SIZE*2]; //shouldnt be more than 128? play with it later

	//setup new user query, more cat games
	strcpy_s(query, SIZE*2, " (username, password, other) VALUES('");
	strcat_s(query, SIZE*2, key.getUserName());
	strcat_s(query, SIZE*2, "','");
	strcat_s(query, SIZE*2, key.getUserPwd());
	strcat_s(query, SIZE*2, "','");
	strcat_s(query, SIZE*2, getOtherInfo());
	strcat_s(query, SIZE*2, "')"); 

	return query;
}

UserKey UserEntry::getKey() {
	return key;
}
