//
// Created by Кирилл Чулков on 2019-05-11.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

const size_t bufSize = 4096;

int main() {
    int sock, listener;
    struct sockaddr_in addr;
    char buf[bufSize];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Can't create socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("Can't bind");
        exit(2);
    }

    listen(listener, 1);

    while (true) {
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("Can't accept");
            exit(3);
        }

        while (true) {
            bytes_read = recv(sock, buf, bufSize, 0);
            if (bytes_read <= 0) break;
            send(sock, buf, bufSize, 0);
        }

        close(sock);
    }

    return 0;
}