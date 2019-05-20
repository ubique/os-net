#include "EchoClient.h"
#include <iostream>

using namespace std;


int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "Wrong arguments" << endl;
		cout << "First argument should be <port>" << endl;
		cout << "Second argument should be <ip address>" << endl;
		exit(EXIT_FAILURE);
	}

	int port = stoi(argv[1]);
	char *ip = argv[2];

	EchoClient client(port, ip);
	client.launch();

	return 0;
}
