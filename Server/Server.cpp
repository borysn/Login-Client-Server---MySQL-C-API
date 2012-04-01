#include "Server.h"

//con/destructors 
Server::Server(int portNum) : REQ_WINSOCK_VER(2), PORT(portNum), SERVER_NAME("localhost") {
	UsersMan = new UsersManager(); 
	started = false; 
	menuLevel = 0; 
} 

Server::~Server() { 
	//nothing really to do here 
} 

//start/stop
//these will handle the server specific menu 
//and options
void Server::Run() {
	while (menuLevel != -1) {
		printServerMenu(); //-1 is the exit code
		menuLevel = processMenu();
	}
}

bool Server::StartServer() {
	//start up the server
	//return false if fails
	cout << "Trying to start server...\n";
	try {
		//all these must be true to start the server succesffult
		if (InitWinSock() && CreateSocket() && BindSocket() != false) {
			cout << "\nStarted the server successfuly...\n";
			return true;
		} else 
			throw Exception("Starting server failed...\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

bool Server::StopServer() {
	//stop server. WSACleanup(...) closesocket(hSocket) closesocket(hClient)
	//return false if fails
	cout << "Trying to stop the server...\n";
	try {
		//cleaup and close up
		if (CleanUpWinSock()) {
			cout << "Stopped the server successfuly...\n";
			return true;
		} else
			throw Exception("Trying to stop the server failed!\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

//retrieval
char *Server::GetHostDescription() {
	char tmp[SIZE] = "\0";
	char port[10] = "\0";

	//get port to char *
	_itoa_s(ntohs(clientSockAddr.sin_port), port, 10, 10);

	strcpy_s(tmp, SIZE, inet_ntoa(clientSockAddr.sin_addr));
	strcat_s(tmp, SIZE, ":");
	strcat_s(tmp, SIZE, port); 

	char *message = new char[SIZE];
	strcpy_s(message, SIZE, tmp);

	return message;
}

IPNumber Server::FindHostIP() {
	LPHOSTENT lpHostent = gethostbyname(SERVER_NAME); 
	try {
		//get hostent structure for hostname
		if (lpHostent == NULL) 
			throw Exception("\n\tCould not resolve hostname.\n");
		//extract primary IP address from hostnet structure
		if (lpHostent->h_addr_list && lpHostent->h_addr_list[0]) { 
			cout << "\n\tHost found.\n"; 
			return inet_addr(inet_ntoa (*(struct in_addr *)*lpHostent->h_addr_list));
				//return *reinterpret_cast<IPNumber*>(lpHostent->h_addr_list[0]);
		}
	} catch (Exception &e) { 
		cout << e.hmm();
	}

	return 0;
} 

void Server::SetServerSockAddr() {
	//set to TCP family, set address, and port number
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = PORT; //DEFAULT_PORT?
	sockAddr.sin_addr.S_un.S_addr = FindHostIP();
}

//setup
bool Server::InitWinSock() { //WSAStartup(...)
	cout << "\n  [Initializing winsock...]";
	try  {
		//make sure we can use winsock and all that
		if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER, 0), &wsaData) == 0) {
			//check for correct winsock version 
			if (LOBYTE(wsaData.wVersion) >= REQ_WINSOCK_VER) {
				cout << "\n\tsuccess.\n";
				return true;
			} else
				throw Exception("\n\trequired version not supported!\n");
		} else {
			throw Exception("\n\tWSAStartup(...) failed!\n");
		}
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

bool Server::CleanUpWinSock() {
	cout <<	"\n  [Cleaning up winsock...]";
	try {
		//cleanup and closeout
		if (mSocket != INVALID_SOCKET)
			closesocket(mSocket);
		if (mClientSock != INVALID_SOCKET)
			closesocket(mClientSock);
		if (WSACleanup() == 0) {
			cout << "\n\tsuccess.\n";
			return true;
		} else
			throw Exception("\n\tfailed!\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

bool Server::CreateSocket() {
	//setup the socket
	//return false if one could not be created
	cout << "\n  [Attempting to create the socket...]";
	try { 
		if ((mSocket = (socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) != INVALID_SOCKET) {
			cout << "\n\tsuccess.\n";
			return true;
		} else
			throw Exception("\n\tfailed!\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

bool Server::BindSocket() {
	//attempt to bind the socket
	//return false if failed
	cout << "\n  [Attempting to bind socket...]"; 
	try {
		SetServerSockAddr();
		//bind
		if (bind(mSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) == 0) {
			cout << "\n\tsuccess.\n";
			return true;
		} else
			throw Exception("\n\tfailed.\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

//functionality
void Server::GetConnection() {
	cout << "\n  [getting a connection...]\n";
	try { 
		int clientSockSize = sizeof(clientSockAddr);
		cout << "\n\tPutting socket in listening mode...";

		//make the socket listen
		if (listen(mSocket, SOMAXCONN) != 0) 
			throw Exception("\n\t\tcould not put socket in listening mode.\n");

		cout << "\n\t\tsuccess.\n"; 

		//set client socket to accepted connection
		mClientSock = accept(mSocket, reinterpret_cast<sockaddr*>(&clientSockAddr),
			                 &clientSockSize); 

		//check if accept succeded 
		if (mClientSock == INVALID_SOCKET) {
			throw Exception("\n\taccept function failed.\n");
		} else 
			cout << "\n\tconnection accepted.\n";
	} catch (Exception &e) {
		cout << e.hmm();
	}
}

void Server::HandleConnection() {
	//this function will handle connection to the client i.e.
	//pass messages back and forth

	cout << "\n\tconnected with " << GetHostDescription() << ".\n";

	int iRet = 1;

	while (iRet != -1) { //iRet = -1 is the disconnect code from the client
						 //          or just if something went wrong
		iRet = getClientRequest();
	}
} 

bool Server::sendMessage(char *pMessage, bool encrypt) {
	//return true if message sending succeds
	//encrypt = true, encypt outbound message via rot 13
	//return false if send fails 
	cout << "\n  [Attempting to send data...]\n";
	try {
		//encrypt the message if specified
		if (encrypt == true)
			rot13(pMessage, SIZE*2);
		//send it out
		if (send(mClientSock, pMessage, SIZE*2, 0) == SOCKET_ERROR)
			throw Exception("\n\tfailed to send data.\n");
		else {
			cout << "\tsuccess.\n";
			return true; 
		}
	} catch (Exception &e) {
		cout << e.hmm();
		return false; 
	}
}

int Server::getClientRequest() {
	//return true if getting a message succeds
	//else return false
	char tmpBuf[SIZE*2] = "\0"; //128
	int retval = 1;
	try { 
		cout << "\n  [Waiting to recieve data...]";
		//try to get data
		int retval = recv(mClientSock, tmpBuf, SIZE*2, 0); 
		if (retval == SOCKET_ERROR) 
			throw Exception("\n\tsocket error while receiving data.\n");
		else { 
			cout << "\n\tdata received...\n";
			//process client request
			retval = processMessage(tmpBuf);
			return retval;
		}
	} catch (Exception &e) {
		cout << e.hmm();
		return -1; //i.e. client drops connection mid dialogue. this handles that
				   //otherwise i get an infinite loop for recieing data when client
		           //drops out mid dialog
	}
	return retval;
}

char *Server::getMessage() {
	//get a message to echo from the client 
	char tmpBuf[SIZE*2] = "\0"; //128
	int retval = 1;
	try { 
		cout << "\n  [Waiting to recieve message...]";
		//try to get data 
		int retval = recv(mClientSock, tmpBuf, SIZE*2, 0);
		if (retval == SOCKET_ERROR) 
			throw Exception("\n\tsocket error while receiving data.\n");
		else { 
			cout << "\n\tmessage received...\n";
		}
	} catch (Exception &e) {
		cout << e.hmm();
		strcpy_s(tmpBuf, SIZE*2, "\0");
	}

	//get buff
	char *message = new char[SIZE*2];
	strcpy_s(message, SIZE*2, tmpBuf);

	return message; 
}

void Server::rot13(char *pBuffer, int size) { //just apply rot 13 to pBuffer
	for (int i = 0; i < size; i++) {
		char c = pBuffer[i];
		if ((c >= 'a' && c < 'n') || (c >= 'A' && c < 'N'))
			c += 13;
		else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
			c -= 13;
		else
			continue;
		pBuffer[i] = c;
	}
}

void Server::printServerMenu() {
	switch (menuLevel) {
	default:
	case 0 : 
		cout << "\n====Server Menu 0====\n";
		cout << "s : Start the server.\n"; 
		cout << "x : Exit application.\n";
		cout << "---------------------\n";
		break; 
	case 1:
		cout << "\n====Server Menu 1====\n";
		cout << "0 : Back to Menu 0.\n";
		cout << "t : Stop the server.\n";
		cout << "w : Wait for connection.\n";
		cout << "x : Exit application.\n";
		cout << "---------------------\n";
		break;
	case 2: 
		cout << "\n====Server Menu 2====\n";
		cout << "0 : Back to Menu 0.\n";
		cout << "1 : Back to Menu 1.\n";
		cout << "t : Stop the server.\n";
		cout << "x : Exit application.\n";
		cout << "---------------------\n";
		break;
	}
}

int Server::processMenu() {
	int level = menuLevel;
	char option;
	cout << ":> "; 
	cin >> option;

	switch (option) {
	//start up the server
	case 's':
		try {
			if (started == false) {
				cout << endl;
				if (StartServer()) { 
					started = true;
					return level+1;
				} else 
					throw Exception("\nError...\n");
			} else
				throw Exception("\nServer already started.\n");
		} catch (Exception &e) {
			cout << e.hmm();
			return level;
		}
		break;
	//stop the server
	case 't':
		try  { 
			if (started == true) {
				cout << endl;
				if (StopServer()) {
					started = false; 
					return 0; //back to 0
				} else
					throw Exception("\nError...\n");
			} else
				throw Exception("\nServer not started.\n");
		} catch (Exception &e) {
			cout << e.hmm();
			return 0; //back to same level
		}
		break;
	//make the server wait for a connection
	case 'w':
		try {
			if (started) {
				//get and handle the connection
				//	i.e. process the connection until its closed
				GetConnection();
				HandleConnection(); 
				//after it's closed we clean up winsock, and re init it
				//then recreate socket, and rebind it
				cout << "\n  [Connection handling finished...]\n";
				//cleanup, reinit, re-listen
				CleanUpWinSock(); 
				InitWinSock();
				CreateSocket();
				BindSocket();
			} else 
				throw Exception("\nServer not started.\n");
		} catch (Exception &e) { 
			cout << e.hmm();
		} 
		return level;
		break;
	//exit the application
	case 'x':
		cout << endl;
		if (started == true)
			StopServer();
		return -1;
		break;
	default:
	//go back to menu 1
	case '1':
		return 1; 
		break;
	//go back to menu 0
	case '0':
		return 0;
		break;
	}
}

//this is another really big function!
//this probably should be split up!
//but ill just leave it big
int Server::processMessage(char *pMessage) {
	try {		
		char name[SIZE] = "\0"; //64
		char pwd[SIZE] = "\0";  //64
		char other[SIZE*2] = "\0"; //128

		cout << "\n  [Processing message...]\n";
		//if exit code
		if (strcmp(pMessage, "-1") == 0) {
			cout << "\tdisconnect code recieved...\n";
			if ((sendMessage("disconnect code recieved...\n", false)) == false)
				throw Exception("\nError sending confirmation...\n"); 
			menuLevel = 1;
			return -1; 
		}
		//if echo message
		else if (strcmp(pMessage, "e") == 0) {
			cout << "\tEcho a message code recieved...\n";
			if ((sendMessage("send message to echo...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			char *message = getMessage();

			if((sendMessage(message, false)) == false) {
				delete [] message;
				throw Exception("\nFailed to send encrypted message...\n");
			}

			delete [] message;

			return 1;
		}
		//if logging out
		else if (strcmp(pMessage, "o") == 0) {
			cout << "\tUser logging out...\n";
			return 1; 
		}
		//if find a user
		else if (strcmp(pMessage, "f") == 0) {
			//send response
			cout << "\tFind user code recieved...\n";
			if ((sendMessage("send username to search for...\n", false)) == false) 
				throw Exception("\nError sending request...\n");

			//get username from user 
			char *message = getMessage(); 
			strcpy_s(name, SIZE, message);

			if (strcmp(name, "\0") == 0)
				throw Exception("\nError getting name...\n");

			cout << "\tName recieved: " << name << endl ; 

			if (UsersMan->userExists(name)) {
				if ((sendMessage("user found!...\n", false)) == false) {
					delete [] message;
					throw Exception("\nError sending message...\n");
				}
			} else {
				delete [] message;
				if ((sendMessage("user not found...\n", false)) == false) 
					throw Exception("\nError sending message...\n");
				throw Exception("\nError...\n");
			}

			delete [] message;

			return 1;
		}
		//if start encryption
		else if (strcmp(pMessage, "k") == 0) {
			cout << "\tEncrypt message code recieved...\n";
			if ((sendMessage("send message to encrypt...\n", false)) == false)
				throw Exception("\nError sending request...\n");
			
			//get message
			char *message = getMessage();

			if((sendMessage(message, true)) == false) {
				throw Exception("\nFailed to send encrypted message...\n");
			}

			delete [] message;

			return 1;
		}
		//if prompted for login
		else if (strcmp(pMessage, "l") == 0) {
			cout << "\tlogin message code recieved...\n";

			//prompt for user name
			if ((sendMessage("send username...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get name
			char *message = getMessage();
			strcpy_s(name, SIZE, message);

			//verify
			if ((strcmp(name, "\0") == 0))
				throw Exception("\nError getting name...\n");
			cout << "\tName recieved: " << name << endl;

			//prompt for password
			if ((sendMessage("send password...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get password
			delete [] message; 
			message = getMessage();
			strcpy_s(pwd, SIZE, message);

			//verify
			if ((strcmp(pwd, "\0") == 0))
				throw Exception("\nError getting password...\n");
			cout << "\tPassword recieved: " << pwd << endl;

			delete [] message;

			UserEntry *pKey = new UserEntry(new UserKey(name, pwd));

			if (UsersMan->login(pKey)) {
				if ((sendMessage("login success...\n", false)) == false) {
					throw Exception("\nError sending request...\n");
				} 
			} else {
				if ((sendMessage("login failed...\n", false)) == false)
					throw Exception("\nError sending request...\n");
			}

			delete pKey;

			return 1;
		}
		//if prompted for registration
		else if (strcmp(pMessage, "r") == 0) {
			cout << "\tregistration message code recieved...\n";

			//prompt for user name
			if ((sendMessage("send username...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get name
			char *message = getMessage();
			strcpy_s(name, SIZE, message);

			//verify
			if ((strcmp(name, "\0") == 0))
				throw Exception("\nError getting name...\n");
			cout << "\tName recieved: " << name << endl;

			//prompt for password
			if ((sendMessage("send password...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get password
			delete [] message;
			message = getMessage();
			strcpy_s(pwd, SIZE, message);

			//verify
			if ((strcmp(pwd, "\0") == 0))
				throw Exception("\nError getting password...\n");
			cout << "\tPassword recieved: " << pwd << endl;
			
			//prompt for other 
			if ((sendMessage("send other info...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get other
			delete [] message;
			message = getMessage();
			strcpy_s(other, SIZE*2, message);

			//verify
			if ((strcmp(other, "\0") == 0)) 
				throw Exception("\nError getting other info...\n");
			cout << "\tOther info recieved: " << other << endl;

			delete [] message;

			UserEntry *entry = new UserEntry(name, pwd, other); 

			if((UsersMan->addNewUser(entry)) == false) {
				delete entry;
				//send back failure 
				if ((sendMessage("adding user failed...\n", false)) == false)
					throw Exception("\nError sending request...\n");
				throw Exception("\nError adding user...\n");
			} else {
				if ((sendMessage("adding user succeeded...\n", false)) == false)
					throw Exception("\nError sending request...\n");
			}

			delete entry;

			return 1;
		}

		//if deleting a user
		else if (strcmp(pMessage, "v") == 0) {
			cout << "\tdelete user message code recieved...\n";

			//prompt for user name
			if ((sendMessage("send username...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get name
			char *message = getMessage();
			strcpy_s(name, SIZE, message);

			//verify
			if ((strcmp(name, "\0") == 0))
				throw Exception("\nError getting name...\n");
			cout << "\tName recieved: " << name << endl;

			//prompt for password
			if ((sendMessage("send password...\n", false)) == false)
				throw Exception("\nError sending request...\n");

			//get password
			delete [] message; 
			message = getMessage();
			strcpy_s(pwd, SIZE, message);

			//verify
			if ((strcmp(pwd, "\0") == 0))
				throw Exception("\nError getting password...\n");
			cout << "\tPassword recieved: " << pwd << endl;

			delete [] message;

			UserKey *pKey = new UserKey(name, pwd);
			UserEntry *pUser = new UserEntry(pKey);

			if (UsersMan->login(pUser)) {
				if ((sendMessage("login success...\n\tdeleting user...\n", false)) == false) {
					throw Exception("\nError sending request...\n");
				} else {
					if (UsersMan->removeUser(pUser)) { 
						strcpy_s(message, SIZE, "user ");
						strcat_s(message, SIZE, pUser->getKey().getUserName());
						strcat_s(message, SIZE, " removed...\n");
						if ((sendMessage(message, false)) == false) 
							throw Exception("\nError sending request...\n");
						delete [] message;
					} else {
						if ((sendMessage("user removal failed...\n", false)) == false) 
							throw Exception("\nError sending request...\n");
					}
				}
			} else {
				//if failes, client is still expecting 2 messages back 
				if ((sendMessage("login failed,\n\tplease try again...\n", false)) == false)
					throw Exception("\nError sending request...\n");
				if ((sendMessage("user removal failed...\n", false)) == false) 
					throw Exception("\nError sending request...\n");
			}

			delete pUser;

			return 1;
		}
		// for everything else disconnect
		// dno how to process it 
		else {
			menuLevel = -1;
			if ((sendMessage("-1", false)) == false)
				throw ("\n\tError sending request...\n");
			throw Exception("  [unhandled message...]\n");
		}
	} catch (Exception &e) {
		cout << e.hmm();
		//menuLevel = 1;
		return menuLevel;
	}
}