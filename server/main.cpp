#include <iostream>
#include "server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Address argument expected" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        server server(argv[1]);
        std::cout << "Echo server.\n";
        server.launch();
    } catch (std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}