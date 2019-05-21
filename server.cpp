#include <iostream>
#include <sstream>

#include "Server.h"

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
        Server server(port);
        server.run();
    } catch (ServerException const& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}