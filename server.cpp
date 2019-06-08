#include "utils.h"

void resolve(const std::string& addr, const std::string& port) {
    int sockfd;
    struct sockaddr_in server;
    utils::init(sockfd, server, addr, port);
    utils::bind_and_listen(sockfd, server);
    while (true) {
        printf("Waiting for connection ...\n");
        struct sockaddr_in user;
        socklen_t len = sizeof(user);
        int resultfd = accept(sockfd, reinterpret_cast<sockaddr*>(&user), &len);
        if (resultfd == -1) {
            perror("Cannot accept connection");
            continue;
        }
        printf("%s connected\n", inet_ntoa(user.sin_addr));
        while (true) {
            printf("--------------------------------------\n");
            printf("Waiting for message ...\n");
            char buffer[utils::BUF_SIZE];
            if (!utils::recv(resultfd, buffer, 1)) {
                printf("Aborting connection\n");
                break;
            }
            char size = buffer[0];
            if (!utils::recv(resultfd, buffer, size)) {
                printf("Aborting connection\n");
                break;
            }
            printf("Message: %s\n", buffer);
            if (!utils::send(resultfd, buffer, size)) {
                printf("Aborting connection\n");
                break;
            }
            printf("Echo message was sent\n");
        }
        utils::close(resultfd);
        printf("++++++++++++++++++++++++++++++++++++++\n");
    }
    utils::close(sockfd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s [addr] [port]\n", argv[0]);
        return 0;
    }
    resolve(argv[1], argv[2]);
}
