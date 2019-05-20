//
// Created by Noname Untitled on 19.05.19.
//

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../Person/Person.h"
#include "../Database/Database.h"
#include "Server.h"

void showUsage(const char* filename) {
    std::cerr << "Usage: " << filename << " <port>";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong arguments. ";
        showUsage(argv[0]);
	std::cerr << std::endl;
        exit(EXIT_FAILURE);
    }

    auto myServer = Server(argv[1]);
    myServer.createBinding();

    myServer.run();

    exit(EXIT_SUCCESS);
}
