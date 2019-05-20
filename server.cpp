//
// Created by rsbat on 5/12/19.
//

#include "POP_server.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: server address port";
        return 0;
    }

    POP_server server(argv[1], argv[2]);

    server.run();
}