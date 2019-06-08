#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "usage : ./server addr port";
        exit(EXIT_FAILURE);
    }

    int port;
    try {
        port = std::stoi(argv[2]);
    } catch (std::invalid_argument &e) {
        std::cout << "Bad port, can't convert to int";
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != 1) {
        perror("Bad adress");
        exit(EXIT_FAILURE);
    }

    if (bind(fd, (sockaddr *) &server, sizeof(server)) == -1) {
        perror("Can't bind");
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 10) == -1) {
        perror("Can't listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server ready!\n";

    std::string hello = "hello ";
    const size_t BUFFER_SIZE = 100;
    char buffer[BUFFER_SIZE + hello.size()];

    for (int i = 0; i < hello.size(); i++) {
        buffer[i] = hello[i];
    }

    struct sockaddr_in client{};
    socklen_t addrlen = sizeof(client);

    bool working = true;
    while (working) {
        int client_fd = accept(fd, (sockaddr *) &client, &addrlen);
        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        std::cout << "Connection established!\n";

        while (working) {
            bzero(buffer + hello.size(), BUFFER_SIZE);

            ssize_t len;
            char last_received = 'a';

            while (last_received != 3) {
                len = recv(client_fd, buffer + hello.size(), BUFFER_SIZE, 0);
                if (len == -1) {
                    perror("Can't receive a message");
                    continue;
                }

                if (len == 0) {
                    break;
                }
                
                last_received = buffer[hello.size() + len - 1];
            }

            if(len == 0){
                continue;
            }
            
            std::string message(buffer + hello.size(), buffer + hello.size() + len - 1);
            std::cout << message << '\n';
            if (message == "stop") {
                working = false;
            }

            int written = 0;
            while (written < len + hello.size()) {
                int currWrtitten = write(client_fd, buffer, len + hello.size());
                if (currWrtitten == -1) {
                    perror("Can't send message.");
                    continue;
                }
                written += currWrtitten;
            }
        }
    }

    close(fd);
}