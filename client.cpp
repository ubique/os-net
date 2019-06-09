//
// Created by Кирилл Чулков on 2019-05-12.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>

extern const size_t buf_size = 2048;
char buf[buf_size + 1];

int main() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Can't create socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("Can't connect");
        exit(2);
    }

//    char message[buf_size];

    std::cout << "Print exit to close program" << std::endl;

    while (true) {

        std::cin.getline(buf, buf_size);
        ssize_t bytes_read = std::cin.gcount();
        buf[bytes_read - 1] = '\0';

        if (strcmp(buf, "exit") == 0) {
            break;
        }

        ssize_t bytes_send = 0;
        while (true) {
            ssize_t nbytes_send = send(sock, buf + bytes_send, bytes_read - bytes_send, 0);
            bytes_send += nbytes_send;
            if (nbytes_send == -1) {
                std::cerr << "Send error";
                break;
            }
            if (bytes_send == bytes_read || nbytes_send == 0) {
                break;
            }
        }

        memset(buf, 0, sizeof(buf));
        bytes_read = 0;
        bool isOk = true;
        while (true) {
            ssize_t nbytes_read = recv(sock, buf + bytes_read, buf_size - bytes_read, 0);
            if (nbytes_read <= 0) {
                std::cerr << "Can't read";
                isOk = false;
                break;
            }
            bytes_read += nbytes_read;
            if (bytes_read == buf_size || buf[bytes_read - 1] == '\0') {
                buf[bytes_read] = '\0';
                ++bytes_read;
                break;
            }
        }

        if (isOk) {
            std::cout << buf << std::endl;
        } else {
            break;
        }

    }

    close(sock);

    return 0;
}