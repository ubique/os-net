#include "EchoClient.h"
#include <vector>

EchoClient::EchoClient(int port, char *ip) {
	openSocket();
	setAddress(port, ip);
	connectSocket();
}

EchoClient::~EchoClient() {
	if (close(sock) < 0) {
		perror("Closing the socket failed");
		exit(EXIT_FAILURE);
	}
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
	sendReq(request);
	readResponse(request);
}

void EchoClient::sendReq(string message) {
	uint8_t len = (uint8_t) message.length();
	const char* request = message.c_str();
	int offset = 0;

	std::vector <uint8_t> length = { len };
	if (send(sock, length.data(), 1, 0) != 1) {
		std::cerr << "Failed to send length of a message" << endl;
		exit(EXIT_FAILURE);
	}

	while (offset != message.length()) {
		int sent = send(sock, request + offset, len, 0);

		if (sent <= 0) {
			std::cerr << "Failed to send request" << endl;
			exit(EXIT_FAILURE);
		}
		
		len -= sent;
		offset += sent;
	}
}

void EchoClient::readResponse(string message) {
	char buffer[1024];
	int received = 0;

	while (received != message.length()) {
		int read = recv(sock, buffer + received, message.length() - received, 0);

		if (read <= 0) {
			std::cerr << "Failed to receive response" << endl;
			exit(EXIT_FAILURE);
		}
		
		received += read;
	}

	buffer[received] = '\0';
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
