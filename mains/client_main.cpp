#include "client/client.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong usage. Two arguments expected: <address> <port>"
                  << std::endl;
        return -1;
    }
    try {
        client client(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        std::cout << "It's echo-client, type anything. Type exit to end it."
                  << std::endl;
        while (!std::cin.eof()) {
            std::cout << "Request: ";
            std::cout.flush();
            std::string text;
            if (text == "exit")
                break;
            std::getline(std::cin, text);
            std::cout << "Responce: " << client.request(text) << std::endl;
        }
    } catch (client_exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
