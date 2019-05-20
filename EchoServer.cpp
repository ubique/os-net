#include "EchoServer.h"

EchoServer::EchoServer(int port, char *ip) {
	openListener();
	setAddress(port, ip);
	if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Socket binding failed");
		exit(EXIT_FAILURE);
	}
	listen(listener, 1);
}

EchoServer::~EchoServer() {
	close(listener);
}

void EchoServer::openListener() {
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		perror("Listener socket opening failed");
		exit(EXIT_FAILURE);
	}
}

void EchoServer::setAddress(int port, char *ip) {
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &(addr.sin_addr));
}

void EchoServer::launch() {
	int socket;
	while (1) {
		socket = accept(listener, NULL, NULL);
		if (socket < 0) {
			perror("accept() error");
			exit(EXIT_FAILURE);
		}

		int bytesRead;
		while (1) {
			char buffer[1024];
			bytesRead = recv(socket, buffer, 1024, 0);
			if (bytesRead <= 0) {
				break;
			}
			buffer[bytesRead] = '\0';
			string str = buffer;
			cout << "Client's request: " << str << endl;
			send(socket, buffer, bytesRead, 0);
		}

		close(socket);
	}
}
