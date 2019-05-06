//
// Created by max on 01.05.19.
//

#include <unistd.h>
#include "client.h"

client::client(std::string host) : host(std::move(host)) {

}


int client::send(const std::string &msg) {
    int ret;

    data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (data_socket == -1) {
        prerror("Unable to create socket");
        return EXIT_FAILURE;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, host.data(), sizeof(addr.sun_path) - 1);

    ret = connect(data_socket, (const struct sockaddr *) &addr,
                  sizeof(struct sockaddr_un));
    if (ret == -1) {
        prerror("The server is down");
        return EXIT_FAILURE;
    }

    ret = write(data_socket, msg.data(), msg.size() + 1);
    if (ret == -1) {
        prerror("Unable to write data");
        return EXIT_FAILURE;
    }

    if (strcasecmp(msg.data(), "exit") != 0) {
        ret = read(data_socket, buffer.data(), BUFFER_SIZE);
        if (ret == -1) {
            prerror("Unable to read");
            return EXIT_FAILURE;
        }

        buffer[BUFFER_SIZE - 1] = 0;

        printf("Result = %s\n", buffer.data());

    }
    close(data_socket);

    return (EXIT_SUCCESS);
}

client::~client() = default;
