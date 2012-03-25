#include "Client.h"

//here we have our main function for the Client application
//this is the only function (besides con/desctructor) that
//can be called outside of this class file
void Client::Run() {
	try { 
		//base menu level
		menuLevel = 0; 
		//menu = -1 here is an exit code
		//dsconnect code for both client and server is -1 as well
		while (menuLevel != -1) {
			//loop application until exit code is recieved 
			printMenu(); 
			menuLevel = processMenu(); 
		} 
	} catch (Exception &e) {
		cout << e.hmm();
	}
}

Client::Client(int portNum, const char *pServerName) : REQ_WINSOCK_VER(2), 
	                                                   SERVER_PORT(portNum), 
													   SERVER_NAME(pServerName) {
	connected = false;
	loggedIn = false; 
	user = NULL; //point to nada
	mSocket = INVALID_SOCKET; 
}

Client::~Client() {
	//nothing really to do here
}

//setup
void Client::FillSockAddr() {
	sockAddr.sin_family = AF_INET; //tcp
	sockAddr.sin_port = SERVER_PORT;
	sockAddr.sin_addr.S_un.S_addr = FindHostIP(); 
}

IPNumber Client::FindHostIP() {
	HOSTENT *pHostent;
	try {
		//get hostent structure for hostname
		if (!(pHostent = gethostbyname(SERVER_NAME))) 
			throw Exception("\n\tCould not resolve hostname.\n");
		//extract primary IP address from hostnet structure
		if (pHostent->h_addr_list && pHostent->h_addr_list[0]) { 
			cout << "\n\tHost found.\n"; 
			return *reinterpret_cast<IPNumber*>(pHostent->h_addr_list[0]);
		}
	} catch (Exception &e) { 
		cout << e.hmm();
	}

	return 0;
}

bool Client::InitWinSock() {
	cout << "  [Initializing winsock...]";
	try  {
		//here we the begining of setting up our network stuff
		if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER, 0), &wsaData) == 0) {
			//check for correct winsock vrsion
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

bool Client::CleanUpWinSock() {	
	//call this at the end of the application
	loggedIn = false; //here?
	try {
		//closeup and cleanup
		cout <<	"\n  [Closing socket...]"; 
		if (mSocket != INVALID_SOCKET) {
			closesocket(mSocket);
			cout << "\n\tsuccess.\n";
		} else
			throw Exception("\n\tsocket is invalid.\n");
		cout <<	"  [Cleaning up winsock...]";
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

bool Client::CreateSocket() {
	try { 
		//now ready to create socket
		cout << "  Creating socket...";
		if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			throw Exception("\n\tcould not create socket.\n");
		cout << "\n\tcreated.\n";
		return true;
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	} 
}

//connection
bool Client::connectToHost() {
	try {
		//init our winsock on run
		if (InitWinSock() == false)
			throw Exception("\nError...\n");

		//find host
		cout << "  Looking for hostname " << SERVER_NAME << "...";
		FillSockAddr(); 

		//create socket
		if (CreateSocket() == false)
			throw Exception("\n\tError...\n");

		//connect to the server
		cout << "  Attempting to connect to " << inet_ntoa(sockAddr.sin_addr)
				<< ":" << SERVER_PORT << "...";

		//if it failes to connect clean up
		if (connect(mSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0) {
			throw Exception("\n\tcould not connect.\n");
		}

		cout << "\n\tconnected.\n"; 
		return true;
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

bool Client::closeConnection() {
	try {
		cout << "\n  Trying to close the connection.\n";
		if (mSocket != INVALID_SOCKET) {
			connected = false;
			//cleanup and reset on disconnect
			CleanUpWinSock();
			cout << "  connection closed.\n";
			return true;
		} else
			throw Exception("  failed...\n");
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

//messages
bool Client::sendMessage(char *pMessage, bool bMenu) {
	try {
		//set menu level to 2
		//this is the send message menu
		//from in here, you must send a message to the server
		//for now there is no way to return to a different menu
		menuLevel = 2;
		if (bMenu)
			printMenu();

		//if a message was not specified get a new message from the keyboard
		char message[SIZE*2]; 
		if (pMessage == NULL) { 
			cout << ":> ";
			//this is a 1 char code
			//from the send message menu
			cin >> message;
		} else //copy specified message an send that
			strcpy_s(message, SIZE*2, pMessage);

		//if failes to send message do no process
		if (send(mSocket, message, SIZE*2, 0) == SOCKET_ERROR)
			throw Exception("\nFailed to send message...\n");
		else {
			//get a response from the server
			//this will either be a message
			//or possibly a response code to que for something else
			//	i.e. a message to encrypt
			if (processResponse(message)) 
				return true;
			else 
				return false;
		}
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
}

const char * Client::getServerResponse() {
	char *tmpBuf = new char[SIZE*2]; 
	try {
		//all we do here is get the response from the server
		//and print it out
		cout << "\n  Processing server response...";
		if ((recv(mSocket, tmpBuf, SIZE*2, 0)) == SOCKET_ERROR)
			throw Exception("\n\tfailed.\n");
		else {
			cout << "\n  Server Response: \n\t" << tmpBuf;
		}
	} catch (Exception &e) {
		cout << e.hmm();
	}
	return tmpBuf;
}

//outputs
void Client::printMenu() {
	switch (menuLevel) {
	case 2:
		//can't back out of this menu right now...
		//is that a problem? seems like it should be definitive
		//to send message after pressing m
		if (loggedIn) { 
			cout << "\n====Logged in menu: " << user->getUserName() << "====\n";
			cout << "k : Begin message encryption.\n";
			cout << "e : Begin echo message.\n";
			cout << "v : Delete a user.\n"; 
			cout << "f : Find a user.\n"; 
			cout << "o : logout.\n"; 
			cout << "-1: Send disconnect.\n";
			cout << "---------------------\n";
		} else {
			cout << "\n====Send Message Menu====\n";
			cout << "r : Prompt to register a user.\n";
			cout << "l : Prompt for login.\n";
			cout << "-1: Send disconnect.\n";
			cout << "---------------------\n";
		}
		break;
	case 1:
		cout << "\n====Client Menu 1====\n";
		cout << "0 : Go back to menu 0.\n";
		cout << "m : Send message.\n";
		cout << "d : Disconnect from host.\n";
		cout << "x : exit application.\n";
		cout << "---------------------\n";
		break;
	default:
	case 0:
		cout << "\n====Client Menu 0====\n";
		cout << "c : Connect to host.\n";
		cout << "x : exit application.\n";
		cout << "---------------------\n";
		break;
	}
}

//process menu option
int Client::processMenu() {
	int level = menuLevel;
	char option;

	//one line char from menu levels 0 or 1
	cout << ":> ";
	cin >> option;

	switch (option) {
	//connect to host
	case 'c':
		try {
			if (connected == false) {
				cout << endl; 
				if(connectToHost()) {
					connected = true;
					return level+1; 
				} else 
					throw Exception("\nError...\n");
			} else
				throw Exception("\nAlready connected...\n"); 
		} catch (Exception &e) {
			cout << e.hmm();
			if(closeConnection() == false)
					throw Exception("\n\tError...\n");
			return level;
		}
		break;
	//send a message to the hose
	case 'm':
		try {
			cout << "\nBringing up Send Message menu...\n";
			if (sendMessage(NULL, true))
				cout << "\n  send message menu returned success.\n";
			else 
				throw Exception("\nError...\n");
		} catch (Exception &e) {
			cout << e.hmm();
			// if  you get an error, server will close connection
			// because the message sent was unhandled.
			// so client side connection should close as well. 
			closeConnection();
			//return to base menu
			return 0;
		}
		return menuLevel;
		break;
	//disconnect from the host
	case 'd':
		try {
			if (connected == true) {
				if(sendMessage("-1", false)) { 
					connected = false;
					return level-1;
				} else
					throw Exception("\nError...\n");
			} else
				throw Exception("\nNot connected to host...\n");
		} catch (Exception &e) {
			cout << e.hmm();
			return level;
		}
		break;
	//exit the application
	case 'x':
		if (connected == true) {
			if(sendMessage("-1", false)) {
				connected = false;
			}
		}
		return -1;
		break;
	//on uknown input just reprint the menu
	case '1':
		return 1;
		break;
	case '2':
		return 2;
		break;
	default:
	case '0':
		return 0;
		break;
	}

	return level;
}

//this is gonna be one massive function
//i could split it up
//i could also just let it be massive
bool Client::processResponse(char *pResponse) {
	//prcoess server response
	try {
		//if gets disconnect code
		if(strcmp(pResponse, "-1") == 0) {
			cout << "\n  Disconnecting...\n";
			getServerResponse();
			menuLevel = 0;
			if(closeConnection() == false)
				throw Exception("\nError...\n");
		}
		//if echoing a message
		else if ((strcmp(pResponse, "e") == 0) && loggedIn) {
			getServerResponse();
			cout << ":> ";
				
			fflush(stdin);
			fgets(pResponse, SIZE*2, stdin);

			//create pointer to \n a end of line input
			char *newline = strchr(pResponse, '\n');
			*newline = '\0'; //replace end of line with \0 

			if (send(mSocket, pResponse, SIZE*2, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send message...\n");
			getServerResponse();
			
			newline = NULL;
			delete newline;

			menuLevel = 1;
		}
		//if find user
		else if ((strcmp(pResponse, "f") == 0) && loggedIn) {
			char name[SIZE] = "\0";

			getServerResponse();
			cout << "Name:> ";
			cin >> name;
			
			if (send(mSocket, name, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send username...\n");

			getServerResponse(); 

			menuLevel = 1; 
		}
		//if prompt for login
		else if (strcmp(pResponse, "l") == 0) {
			char name[SIZE] = "\0";
			char pwd[SIZE] = "\0";
			
			//get and send user name
			getServerResponse(); 
			cout << "Name:> ";
			cin >> name;
			if (send(mSocket, name, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send user name...\n");

			//get and send password
			getServerResponse();
			cout << "Password:> ";
			cin >> pwd; 
			if (send(mSocket, pwd, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send password...\n");

			user = new UserKey(name, pwd); 

			//get success or not
			//if success loggedIn = true
			//cout << "\n\nlogin success...\n" << getServerResponse() << endl;
			//pResponse = getServerResponse();
			string response = string(getServerResponse());
			if (response.find("success") != string::npos) {
				loggedIn = true; 
			} 
			
			menuLevel = 1;
		}

		//if logging out 
		else if ((strcmp(pResponse, "o") == 0) && loggedIn) {
			cout << "\n  Logging user out...\n";
			delete [] user; 
			loggedIn = false;
			menuLevel = 1;
		}

		//if prompt for registrations
		else if (strcmp(pResponse, "r") == 0) {
			char name[SIZE] = "\0";
			char pwd[SIZE] = "\0";
			char other[SIZE*2] = "\0";

			//get and send user name
			getServerResponse(); 
			cout << "Name:> ";
			cin >> name;
			if (send(mSocket, name, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send user name...\n");

			//get and send password
			getServerResponse();
			cout << "Password:> ";
			cin >> pwd;
			if (send(mSocket, pwd, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send password...\n");

			//get and send password
			getServerResponse();
			cout << "Other:> "; 
			fflush(stdin); 
			fgets(other, SIZE*2, stdin);

			//change end of line char from input
			char *newline = strchr(other, '\n');
			*newline = '\0';

			if (send(mSocket, other, SIZE*2, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send other...\n");

			newline = NULL;
			delete newline;

			getServerResponse(); 

			menuLevel = 1;
		}
		//if encryting a message
		else if ((strcmp(pResponse, "k") == 0) && loggedIn) { 
			getServerResponse();
			cout << ":> ";
				
			fflush(stdin);
			fgets(pResponse, SIZE*2, stdin);

			//create pointer to \n a end of line input
			char *newline = strchr(pResponse, '\n');
			*newline = '\0'; //replace end of line with \0 

			if (send(mSocket, pResponse, SIZE*2, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send message...\n");
			getServerResponse();
			
			newline = NULL;
			delete newline;

			menuLevel = 1;
		}
		//if deleteing a user
		else if ((strcmp(pResponse, "v") == 0) && loggedIn) { 
			char name[SIZE] = "\0";
			char pwd[SIZE] = "\0";
			
			//get and send user name
			getServerResponse(); 
			cout << "Name:> ";
			cin >> name;
			if (send(mSocket, name, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send user name...\n");

			//get and send password
			getServerResponse();
			cout << "Password:> ";
			cin >> pwd;
			if (send(mSocket, pwd, SIZE, 0) == SOCKET_ERROR)
				throw Exception("\nFailed to send password...\n");

			//login must succeed in order to remove user
			//get success or not
			getServerResponse();

			//add y/n final decision here. 

			//get removal seuccess or not 
			//if a user deletes him/her..self then log the user out!
			string response = string(getServerResponse()); 
			if(response.find(user->getUserName()) != string::npos) { 
				processResponse("o"); 
			} 

			menuLevel = 1;
		}
		//and for everything else just disconnect
		//dno how to handle it
		else {
			cout << "\n  Disconnecting...\n";
			menuLevel = 0;
			if(closeConnection() == false)
				throw Exception("\nError...\n");
		}
	} catch (Exception &e) {
		cout << e.hmm();
		return false;
	}
	return true;
}