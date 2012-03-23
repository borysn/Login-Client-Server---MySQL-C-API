#include "main.h"

int main() {
	int port;
	cout << "Creating a Server object...\nEnter port #: ";
	cin >> port;

	Server *server = new Server(port);
	server->Run();
	delete server;

	return 0;
}