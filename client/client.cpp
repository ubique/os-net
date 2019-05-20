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

char buf[1024];

int main() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(110);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    char message[1024];
    int bytes_read;

    while (true) {

        std::cin.getline(message, 1024);
//        std::cin >> message;
        send(sock, message, sizeof(message), 0);
        bytes_read = recv(sock, buf, 1024, 0);
        printf("%s", buf);
        while (bytes_read == 1024) {
//            bytes_read = recv(sock, buf, 1024, 0);
            bytes_read = read(sock, buf, 1024);
            printf("%s", buf);
        }
        if (strcmp(message, "QUIT") == 0) {
            break;
        }
    }

    close(sock);

    return 0;
}