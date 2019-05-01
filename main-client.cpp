//
// Created by max on 01.05.19.
//

#include "client.h"

int main(int argc, char **argv) {
    client client1{(argc != 1 ? argv[1] : "/tmp/server.socket")};
    std::string request;
    while (request != "exit") {
        std::cin >> request;
        client1.send(request);
    }
}