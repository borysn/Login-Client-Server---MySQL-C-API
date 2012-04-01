#include "main.h"

int main() {
	string server;
	int portNum;

	cout << "Creating a client object...";
	
	bool getInput = true;
	while (getInput) {
		cout << "\nEnter server name: ";
		cin >> server;
		cout << "Enter port #: ";
		cin >> portNum;

		cout << "\nServer Name: " << server.c_str() << endl;
		cout << "Port #: " << portNum << endl;
		cout << "Is this correct? y/n: ";

		char c;
		cin >> c; 

		switch (c) {
		case 'y':
		case 'Y': 
			getInput = false;
			break;
		case 'n':
		case 'N':
		default:
			portNum = 0;
			getInput = true;
			break;
		}
	}

	Client *client = new Client(portNum, server.c_str()); 
	client->Run();
	delete client;

	return 0;
}