//
// Created by roman on 20.05.19.
//
#include "hello_server.h"

#include <iostream>

int main(int argc, char *argv[]) {
    hello_server server;
    try {
        server.start(argv[1], std::atoi(argv[2]));
    } catch (std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}
