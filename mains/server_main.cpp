#include "server/server.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong usage. 2 arguments expected: <adress> <port>"
                  << std::endl;
        return -1;
    }
    try {
        server server(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        server.run();
    } catch (server_exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
