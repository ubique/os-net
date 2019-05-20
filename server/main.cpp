//
// Created by vitalya on 19.05.19.
//

#include "server.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Address argument expected" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        server server(argv[1]);
        server.start();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
