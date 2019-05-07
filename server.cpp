//
// Created by max on 01.05.19.
//

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <memory>
#include "server.h"

server::server(const std::string &soc_name) {
    socket.create(soc_name, Socket::FLAG_SERVER);
}


int server::start() {
    socket.bind();

    socket.listen();

    while (true) {

        socket.accept();

        auto data = socket.read();

        std::cout << "read" << std::endl;
        std::cout << data.second.get() << std::endl;
        if (strcasecmp(data.second.get(), "exit") != 0) {
            std::string str(adding);
            str.append(data.second.get());
            socket.write(str);
        } else {
            std::cout << "Exiting..." << std::endl;
            break;
        }
    }
    return EXIT_SUCCESS;
}

server::~server() = default;