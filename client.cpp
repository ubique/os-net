#include "utils.h"
#include <cstring>

void resolve(const std::string& addr, const std::string& port) {
    int sockfd;
    struct sockaddr_in server;
    utils::init(sockfd, server, addr, port);
    utils::connect(sockfd, server);
    printf("Connected to server\n");
    while (true) {
        printf("Request: ");
        char buffer[utils::BUF_SIZE];
        if (scanf("%255s", buffer) == EOF) {
            printf("\n");
            break;
        }
        char size = strlen(buffer) + 1;
        if (!utils::send(sockfd, &size, 1)) {
            printf("Aborting connection\n");
            break;
        }
        if (!utils::send(sockfd, buffer, size)) {
            printf("Aborting connection\n");
            break;
        }
        printf("Message is sent\n");
        if (!utils::recv(sockfd, buffer, size)) {
            printf("Aborting connection\n");
            break;
        }
        printf("Response: %s\n", buffer);
    }
    utils::close(sockfd);
    printf("Disconnected from server\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s [addr] [port]\n", argv[0]);
        return 0;
    }
    resolve(argv[1], argv[2]);
}

