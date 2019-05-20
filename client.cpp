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

const size_t bufSize = 4096;
char buf[bufSize];

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

    char message[bufSize];

    std::cout << "Print exit to close program" << std::endl;

    while (true) {

        std::cin.getline(message, bufSize);
        if (strcmp(message, "exit") == 0) {
            break;
        }

        if (send(sock, message, sizeof(message), 0) == -1) {
            std::cerr << "Can't send message" << std::endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
        if (recv(sock, buf, bufSize, 0) == -1) {
            std::cerr << "Can't read" << std::endl;
            break;
        }
        std::cout << buf << std::endl;
    }

    close(sock);

    return 0;
}