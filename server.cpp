#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>


void handle(int fd) {
    char tmp[1024];
    int nread;
    std::string buffer;

    while ((nread = read(fd, tmp, sizeof(tmp))) > 0 || errno == EINTR) {
        std::copy(tmp, tmp + nread, std::inserter(buffer, buffer.end()));

        if (std::find(tmp, tmp + nread, '\n') != tmp + nread) {
            *std::find(buffer.begin(), buffer.end(), '\n') = 0;
            break;
        }
    }

    if (nread < 0) {
        fprintf(stderr, "WARN: could not read from socket: %s\n", strerror(errno));
        return;
    }

    std::string reply = "Got: ";
    reply += buffer.substr(0, strlen(buffer.c_str()));
    reply += '\n';

    int nwrtn;
    size_t offset = 0;
    while ((nwrtn = write(fd, reply.c_str() + offset, reply.size() - offset)) > 0 || errno == EINTR) {
        offset += nwrtn;
    }

    if (nwrtn < 0) {
        fprintf(stderr, "WARN: could not write to socket: %s\n", strerror(errno));
        return;
    }

    close(fd);

}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: os_server <port>\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (sock < 0) {
        fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
        return 1;
    }

    {
        struct sockaddr_in addr_server;

        addr_server.sin_family = AF_INET;
        addr_server.sin_addr.s_addr = INADDR_ANY;
        addr_server.sin_port = htons(atoi(argv[1]));

        int ret = bind(sock, (sockaddr *) &addr_server, sizeof(addr_server));

        if (ret < 0) {
            fprintf(stderr, "Could not bind socket: %s\n", strerror(errno));
            return 0;
        }
    }

    while (true) {
        sockaddr_in addr_client;
        socklen_t addr_len;

        int ret = listen(sock, 3);

        if (ret < 0) {
            fprintf(stderr, "Listen failed: %s\n", strerror(errno));
            break;
        }

        int fd = accept(sock, (sockaddr *) &addr_client, &addr_len);

        if (ret < 0) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
        }

        handle(fd);
    }

    return 0;
}