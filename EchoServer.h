#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

using namespace std;

class EchoServer {
	public:
		EchoServer(int, char *);
		~EchoServer();

		void openListener();
		void setAddress(int, char *);
		void launch();

	private:
		int listener;
		struct sockaddr_in addr;
		bool sendResponse(int, string);
		bool readRequest(int, int, string&);
	
};
