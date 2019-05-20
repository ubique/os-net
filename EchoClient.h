#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

class EchoClient {
	public:
		EchoClient(int, char *);
		~EchoClient();

		void connectSocket();
		void openSocket();
		void setAddress(int, char *);
		void sendRequest(string);
		void launch();
	private:
		int sock;
		struct sockaddr_in addr;
};
