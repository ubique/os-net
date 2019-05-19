#include <iostream>

#include "Server.h"


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "wrong number of arguments, required: [address] [port]" << std::endl;
        return EXIT_FAILURE;
    }
    uint16_t port;
    try {
        port = static_cast<uint16_t >(std::stoul(argv[2]));
    }
    catch (std::invalid_argument &e) {
        std::cerr << "argument should be a positive number" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        Server server(argv[1], port);
        server.start();
    }
    catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
