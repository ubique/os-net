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


#import "POP3Server.h"

int main() {
    int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;
    POP3Server pop3Server;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(110);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while (true) {
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        while (true) {
            bytes_read = recv(sock, buf, 1024, 0);
            if (bytes_read <= 0) break;
            std::string response = pop3Server.processCommand(buf);
//            printf("%s", response.c_str());
            send(sock, response.c_str(), response.size(), 0);
        }

        close(sock);
    }

    return 0;
}