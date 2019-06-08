#include "EchoServer.h"
#include <vector>

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
	if (close(listener) < 0) {
		perror("Closing the socket failed");
		exit(EXIT_FAILURE);
	}
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

bool EchoServer::readRequest(int socket, int length, string& request) {
	char buffer[length];
	int received = 0;

	while (received != length) {
		int read = recv(socket, buffer + received, length - received, 0);

		if (read <= 0) {
			std::cerr << "Failed to receive request" << endl;
			return false;
		}

		received += read;
	}

	buffer[received] = '\0';
	request = buffer;
	return true;
}

bool EchoServer::sendResponse(int socket, string message) {
	const char* response = message.c_str();
	int offset = 0;
	int len = message.length();

	while (offset != message.length()) {
		int sent = send(socket, response + offset, len, 0);

		if (sent <= 0) {
			std::cerr << "Failed to send response" << endl;
			return false;
		}

		len -= sent;
		offset += sent;
	}

	return true;
}

void EchoServer::launch() {
	int socket;
	while (1) {
		socket = accept(listener, NULL, NULL);
		if (socket < 0) {
			perror("accept() error");
			exit(EXIT_FAILURE);
		}

		while (1) {
			std::vector <uint8_t> request_length(1);

			int read_length = recv(socket, request_length.data(), 1, 0);

			if (read_length != 1) {
				std::cerr << "Failed to receive length of the request" << endl;
				break;
			}

			int length = request_length[0];

			string request;
			if (!readRequest(socket, length, request)) {
				break;
			}

			cout << "Client's request: " << request << endl;
			
			if (!sendResponse(socket, request)) {
				break;
			}
		}

		if (close(socket) < 0) {
			perror("Closing the socket failed");
			exit(EXIT_FAILURE);
		}
	}
}
