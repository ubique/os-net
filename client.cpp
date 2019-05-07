//
// Created by max on 01.05.19.
//

#include <unistd.h>
#include "client.h"

client::client(std::string host) : host(std::move(host)) {

}


int client::send(const std::string &msg) {
    socket.create(host, Socket::FLAG_CLIENT);

    socket.connect();

    socket.write(msg);


    if (strcasecmp(msg.data(), "exit") != 0) {
        auto read_ans = socket.read();

        printf("Result = %s\n", read_ans.second.get());

    }

    return (EXIT_SUCCESS);
}

client::~client() = default;
