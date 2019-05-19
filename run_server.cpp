#include "server.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Expected 2 arguments: <address> <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    try {
        server server(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        server.run();
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid port" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}