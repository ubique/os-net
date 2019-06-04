//
// Created by roman on 20.05.19.
//
#include "server.h"

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [Internet host address] [port]" << std::endl;
        return 0;
    }
    server server;
    server.start(argv[1], std::atoi(argv[2]));
    return 0;
}
