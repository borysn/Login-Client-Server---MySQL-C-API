#include "main.h"

int main() {
	int port;
	cout << "Creating a Server object...\n";

	bool getInput = true;
	while (getInput) {
		cout << "Enter port #: ";
		cin >> port;

		cout << "\nPort #: " << port;
		cout << "\nIs this correct? y/n: ";

		char c = ' ';
		cin >> c;

		switch (c) {
		case 'y':
		case 'Y': 
			getInput = false;
			break;
		default:
		case 'n': 
		case 'N':
			port = -1;
			c = ' ';
			getInput = true;
			break;
		}
	}

	Server *server = new Server(port);
	server->Run();
	delete server;

	return 0;
}