//
// Created by daniil on 19.05.19.
//

#include <iostream>
#include "Client.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Three arguments expected" << std::endl;
        return 1;
    }
    try {
        Client client(argv[1], static_cast<uint16_t >(std::stoul(argv[2])));
        std::string text;
        while (std::getline(std::cin, text)) {
            if (text == "exit") {
                break;
            }
            std::cout << "Echo: " << client.request(text) << std::endl;
        }
    } catch (client_exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
