#include <iostream>
#include <sstream>
#include "Client.h"

int main() {
    Client client(8888);
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
        std::cout << response << std::endl;
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
            std::cout << response << std::endl;
        }
    }
}