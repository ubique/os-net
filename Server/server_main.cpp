//
// Created by Noname Untitled on 19.05.19.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../Person/Person.h"
#include "../Database/Database.h"
#include "Server.h"

void showUsage(const char *filename) {
    std::cerr << "Usage: " << filename << " <port>" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong arguments. ";
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    auto myServer = Server(argv[1]);
    myServer.createBinding();

    myServer.run();

    exit(EXIT_SUCCESS);
}
