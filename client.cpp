#include <iostream>
#include <sstream>
#include "Client.h"

void printFormat() {
    std::cout << "Please, provide port number as the first argument" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printFormat();
        return 0;
    }
    uint16_t port;
    std::stringstream argument(argv[1]);
    if (!(argument >> port)) {
        printFormat();
        return 0;
    }
    try {
        Client client(port);
        client.run();
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "--help") {
                std::cout << "Мне бы кто помог" << std::endl;
                continue;
            }
            line.append("\r\n");
            client.sendRequest(line);
            auto response = client.getResponse();
            std::cout << response;
            if (response[0] == '3') {
                std::string block;
                std::stringstream multiblock;
                while (std::getline(std::cin, block)) {
                    if (block == ".") {
                        break;
                    }
                    multiblock << block << "\r\n";
                }
                multiblock << ".\r\n";
                client.sendRequest(multiblock.str());
                response = client.getResponse();
                std::cout << response;
            }
        }
    } catch (ClientException const& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}