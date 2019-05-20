//
// Created by Михаил Терентьев on 2019-05-13.
//

#include <iostream>
#include "../server-lib/server.h"
#include "../server-lib/server_execption.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid args\n expected 2 args: [address] [port]\n";
        return -1;
    }
    try {
        server server(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        server.run();
    } catch (server_exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}