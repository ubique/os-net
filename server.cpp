#include <iostream>
#include "POP3Server.h"

int main(int argc, char** argv) {

    if (argc == 0 || argc > 2) {
        std::cerr << "Incorrect count of arguments!" << std::endl;
        exit(EXIT_FAILURE);
    }
    POP3Server server(argc == 1 ? argv[0] : argv[1],  8888);
    server.run();
}

