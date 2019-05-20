//
// Created by daniil on 19.05.19.
//

#include <iostream>
#include "Server.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "3 arguments excepted" << std::endl;
        return 1;
    }
    try {
        Server server(argv[1], static_cast<uint16_t >(std::stoul(argv[2])));
        server.run();
    } catch (server_exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
