#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void close_socket(int fd) {
    if (close(fd) < 0)
        perror("Cannot close socket");
}

void print_error_and_exit(string message, int fd) {
    perror(message.c_str());
    close_socket(fd);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        cout << "Usage: server <addr> <port>\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (!inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)) {
        cout << "Bad address " << argv[1] << '\n';
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create file descriptor for socket");
        exit(EXIT_FAILURE);
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof yes) < 0)
        print_error_and_exit("Cannot reuse addr", sockfd);
    if (bind(sockfd, (struct sockaddr*)  &serv_addr, sizeof serv_addr) < 0)
        print_error_and_exit("Cannot bind to socket", sockfd);
    if (listen(sockfd, 10) < 0)
        print_error_and_exit("Listening failed", sockfd);

    while (true) {
        struct sockaddr_in cli_addr;
        socklen_t addrlen = sizeof cli_addr;
        int new_fd = accept(sockfd, (struct sockaddr*) &cli_addr, &addrlen);
        if (new_fd < 0) {
            perror("Accepting connection failed");
            close_socket(new_fd);
            continue;
        }
        char buf[101];
        int sz = recv(new_fd, buf, 100, 0);
        if (sz < 0) {
            perror("Receiving failed");
            close_socket(new_fd);
            continue;
        }
        buf[sz] = '\0';
        string message;
        if (strcmp(buf, "close") != 0)
            message = "Hello, " + string(buf) + "!";
        else
            message = "Bye!";
        if (send(new_fd, message.data(), message.size(), 0) < 0) {
            perror("Sending failed");
            close_socket(new_fd);
            continue;
        }
        close_socket(new_fd);
        if (strcmp(buf, "close") == 0)
            break;
    }
    close_socket(sockfd);
    return 0;
}

