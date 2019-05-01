//
// Created by max on 01.05.19.
//

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <memory>
#include "server.h"

server::server(std::string soc_name) : soc_name(std::move(soc_name)) {
    unlink(soc_name.data());
    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        throw std::runtime_error("Unable to create socket");
    }
}


int server::start() {
    struct sockaddr_un name{};
    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, soc_name.data(), sizeof(name.sun_path) - 1);
    int bind_ret = bind(connection_socket, reinterpret_cast<const struct sockaddr *>(&name),
                        sizeof(struct sockaddr_un));
    if (bind_ret == -1) {
        prerror("Unable to bind");
        return EXIT_FAILURE;
    }

    int ret = listen(connection_socket, 100);
    if (ret == -1) {
        prerror("Unable to listen to socket");
        return EXIT_FAILURE;
    }
    int prev_data_socket = -1;
    while (true) {
        data_socket = accept(connection_socket, nullptr, nullptr);
        if (data_socket == -1) {
            prerror("Unable to accept");
            return EXIT_FAILURE;
        }
        if (prev_data_socket != -1) {
            close(prev_data_socket);
            std::cout << "closed" << std::endl;
        }
        std::cout << "acc" << std::endl;
        int read_ret = read(data_socket, buffer.data(), BUFFER_SIZE);
        if (read_ret == -1) {
            prerror("Unable to listen to socket");
            return EXIT_FAILURE;
        }
        std::cout << "read" << std::endl;
        std::cout << buffer.data() << std::endl;
        if (strcasecmp(buffer.data(), "exit") != 0) {
            std::unique_ptr<char> str(new char[BUFFER_SIZE]);
            std::unique_ptr<char> helper(new char[BUFFER_SIZE]);
            memmove(helper.get(), buffer.data(), BUFFER_SIZE);
            memcpy(str.get(), adding.data(), adding.size());
            memcpy(str.get() + adding.size(), helper.get(), BUFFER_SIZE - adding.size());
            str.get()[BUFFER_SIZE - 1] = 0;
            int ret_write = write(data_socket, str.get(), BUFFER_SIZE);
            if (ret_write == -1) {
                prerror("Unable to write");
                return EXIT_FAILURE;
            }
        } else {
            std::cout << "Exiting..." << std::endl;
            break;
        }
        prev_data_socket = data_socket;
    }
    close(data_socket);

    return EXIT_SUCCESS;
}

server::~server() {
    close(connection_socket);
    unlink(soc_name.data());
    std::cout << "Server down" << std::endl;
}