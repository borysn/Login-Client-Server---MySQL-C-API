#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include <Windows.h>
#include "UserEntry.h"
#include "UsersManager.h"

using namespace std;

#define WIN32_LEAN_AND_MEAN //arrrggh
#define SIZE 64

typedef unsigned long IPNumber;

class UsersManager;

class Server {
private:
	bool started;
	int menuLevel;
	const int REQ_WINSOCK_VER;
	const int PORT;
	const char *SERVER_NAME;
	UsersManager *UsersMan;
	sockaddr_in clientSockAddr;
	sockaddr_in sockAddr; 
	SOCKET mClientSock;
	SOCKET mSocket;
	WSAData wsaData;

	//private functions

	//start/stop
	bool StartServer();
	bool StopServer();

	//setup
	//IPNumber Client::FindHostIP()
	char *GetHostDescription();
	IPNumber FindHostIP();
	bool InitWinSock();
	bool CleanUpWinSock();
	void SetServerSockAddr();
	bool CreateSocket();
	bool BindSocket();

	//connection management
	void GetConnection();
	void HandleConnection();

	//message management
	bool sendMessage(char *pMessage, bool encrypt);
	int getClientRequest();
	char *getMessage();
	int processMessage(char *pMessage);
	void rot13(char *pBuffer, int size);

	//menus n such
	void printServerMenu();
	int processMenu();
public:
	//con/destructors
	Server(int portNum);
	~Server();
	
	//Go!
	void Run();
};

#endif