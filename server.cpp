#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

void print_usage() {
    cout << "Usage: server [address] [port]" << endl;
}

void close_socket(int fd) {
    if (close(fd) < 0) {
        perror("Cannot close socket");
    }
}

const int MAX_LEN = 100;

const int BUFF_SIZE = 1024;

int main(int argc, char **argv, char **env) {
    if (argc != 3) {
        print_usage();
        return 0;
    }
    int sockfd, port;
    struct sockaddr_in address;

    try {
        port = stoull(argv[2]);
    } catch (exception &e) {
        cout << "Invalid port: " << argv[2] << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (!inet_pton(AF_INET, argv[1], &address.sin_addr)) {
        cout << "Invalid domain: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

    int opt = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close_socket(sockfd);
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("binding failed");
        close_socket(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 3) < 0) {
        perror("listening failed");
        close_socket(sockfd);
        exit(EXIT_FAILURE);
    }

    cout << "Listening at " << argv[1] << ":" << argv[2] << endl;

    while (true) {
        int newsocket;
        struct sockaddr_in client_address;
        socklen_t addrlen = sizeof(client_address);
        if ((newsocket = accept(sockfd, (struct sockaddr *) &client_address,
                                &addrlen)) < 0) {
            perror("accept");
            close_socket(newsocket);
            continue;
        }
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), addr, INET_ADDRSTRLEN);
        cout << "Accepted connection from " << addr << endl;
        char buffer[BUFF_SIZE] = {0};
        int valread, received = 0;
        while ((valread = recv(newsocket, buffer + received, BUFF_SIZE - received, 0)) > 0) {
            received += valread;
            if (buffer[received - 1] == '\n') {
                break;
            }
        }
        if (valread < 0) {
            perror("Receiving failed");
            close_socket(newsocket);
            continue;
        }

        cout << "Received: " << buffer;
        int length = strlen(buffer), sent = 0;
        bool err = false;
        while (sent < length && !err) {
            cout.flush();
            int cnt = send(newsocket, buffer + sent, length - sent, 0);
            if (cnt < 0) {
                err = true;
            }
            sent += cnt;
        }
        if (err) {
            perror("Sending failed");
            close_socket(newsocket);
            continue;
        }

        cout << "Echo sent" << endl;
        close_socket(newsocket);

        if (strcmp(buffer, "stop\n") == 0) {
            cout << "Server stopped" << endl;
            break;
        }
    }
    close_socket(sockfd);
}