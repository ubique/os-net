#include <cstdlib>
#include <iostream>
#include <string>

#include "client.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Client usage: <address> <port> <message>" << std::endl;
        exit(EXIT_FAILURE);
    }

    try {
        client cl({argv[1]}, {argv[2]});
        cl.connect_to_server();
        cl.work(std::string(argv[3]));
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    }
}
