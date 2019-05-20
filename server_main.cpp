//
// Created by roman on 20.05.19.
//
#include "hello_server.h"

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "server [Internet host address] [port]" << std::endl;
        return 0;
    }
    try {
        hello_server server;
        try {
            server.start(argv[1], std::atoi(argv[2]));
        } catch (std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    } catch (std::runtime_error &constr_er) {
        std::cerr << constr_er.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
