//
// Created by roman on 20.05.19.
//
#include "server.h"

#include <iostream>

int main(int argc, char *argv[]) {
    const char* server_address = "127.0.0.1";
    int port = 8080;
    if (argc == 3) {
        server_address = argv[1];
        port = std::atoi(argv[2]);
    }
    server server;
    server.start(server_address, port);
    return 0;
}
