#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include "Exception.h"

using namespace std;

#define WIN32_LEAN_AND_MEAN
#define SIZE 64

typedef unsigned long IPNumber; 

class Client {
private:
	bool connected;
	bool loggedIn;
	int menuLevel;
	const int REQ_WINSOCK_VER;
	const int SERVER_PORT;
	const char *SERVER_NAME;
	sockaddr_in sockAddr; 
	SOCKET mSocket;
	WSAData wsaData;

	//private functions...
	//setup
	void Client::FillSockAddr();
	IPNumber Client::FindHostIP();
	bool Client::InitWinSock();
	bool Client::CleanUpWinSock();
	bool Client::CreateSocket();

	//connection management
	bool Client::connectToHost();
	bool Client::closeConnection();

	//message management
	bool Client::sendMessage(char *pMessage, bool bMenu);
	const char *Client::getServerResponse();
	bool Client::processResponse(char *pResponse);

	//menu and stuff
	void Client::printMenu();
	int Client::processMenu();
public:
	//con/destructors
	Client(int portNum, const char *pServerName);
	~Client();
	//Go!
	void Run();
};

#endif