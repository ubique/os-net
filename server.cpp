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
#include <iostream>

const size_t buf_size = 2048;

int main() {
    int sock, listener;
    struct sockaddr_in addr;
    char buf[buf_size + 1];

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
            ssize_t bytes_read = 0;
            bool isOk = true;
            while (true) {
                ssize_t nbytes_read = recv(sock, buf + bytes_read, buf_size - bytes_read, 0);
                if (nbytes_read <= 0) {
                    std::cerr << "Error in read";
                    isOk = false;
                    break;
                }
                bytes_read += nbytes_read;
                if (bytes_read == buf_size || buf[bytes_read - 1] == '\0') {
                    buf[bytes_read] = '\0';
                    break;
                }
            }

            if (!isOk) {
                break;
            }

            ssize_t bytes_send = 0;
            while (true) {
                ssize_t nbytes_send = send(sock, buf + bytes_send, bytes_read - bytes_send, 0);
                bytes_send += nbytes_send;
                if (nbytes_send == -1) {
                    std::cerr << "Send error";
                    isOk = false;
                    break;
                }
                if (bytes_send == bytes_read || nbytes_send == 0) {
                    break;
                }
            }

            if (!isOk) {
                break;
            }

        }

        close(sock);
    }
}