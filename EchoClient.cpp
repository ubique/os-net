#include "EchoClient.h"

EchoClient::EchoClient(int port, char *ip) {
	openSocket();
	setAddress(port, ip);
	connectSocket();
}

EchoClient::~EchoClient() {
	close(sock);
}


void EchoClient::connectSocket() {
	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Socket connection failed");
		exit(EXIT_FAILURE);
	}
}

void EchoClient::openSocket() {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Socket opening failed");
		exit(EXIT_FAILURE);
	}
}

void EchoClient::setAddress(int port, char *ip) {
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &(addr.sin_addr));
}

void EchoClient::sendRequest(string request) {
	char buffer[1024];
	send(sock, request.c_str(), request.length(), 0);
	int bytesRead = recv(sock, buffer, 1024, 0);
	buffer[bytesRead] = '\0';
	string str = buffer;
	cout << "Server response: " << buffer << endl;
}

void EchoClient::launch() {
	cout << "Enter 'exit' or 'quit' to stop" << endl;
	while (1) {
		cout << "Enter request: ";
		string request;
		cin >> request;
		if ((strcmp(request.c_str(), "exit") == 0) || (strcmp(request.c_str(), "quit") == 0)) {
			break;
		}
		sendRequest(request);
	}
}
