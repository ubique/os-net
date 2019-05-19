#include <iostream>

#include "Client.h"


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
        Client client(argv[1], port);
        std::string message;
        std::cout << "print \"ex\" to finish program" << std::endl;
        while (!std::cin.eof()) {
            std::cout << "your request: " << std::endl;
            std::getline(std::cin, message);
            if (message == "ex") {
                break;
            }
            std::cout << "answer from server:" << std::endl;
            std::cout << client.makeRequest(message) << std::endl;
        }
    }
    catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
