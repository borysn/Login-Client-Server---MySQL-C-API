#include "main.h"

int main() {
	string server;
	int portNum;

	cout << "Creating a client object...\nEnter server name: ";
	getline(cin, server); 
	cout << "Enter port #: ";
	cin >> portNum;

	Client *client = new Client(portNum, server.c_str()); 
	client->Run();
	delete client;

	return 0;
}