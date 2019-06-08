#include "utils.h"
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace utils {
    void connect(int& sockfd, sockaddr_in& server) {
        if (::connect(sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(sockaddr_in)) == -1) {
            perror("Cannot connect to server");
            exit(EXIT_FAILURE);
        }
    }

    void bind_and_listen(int& sockfd, sockaddr_in& server) {
        if (bind(sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(sockaddr_in)) == -1) {
            perror("Cannot bind to socket");
            exit(EXIT_FAILURE);
        }
        if (listen(sockfd, BACKLOG) == -1) {
            perror("Cannot listen socket");
            exit(EXIT_FAILURE);
        }
    }

    void init(int& sockfd, sockaddr_in& server, const std::string& addr, const std::string& port) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("Cannot create socket\n");
            exit(EXIT_FAILURE);
        }
        server.sin_family = AF_INET;
        if (inet_aton(addr.data(), &server.sin_addr) == 0) {
            printf("Server's address is invalid\n");
            exit(EXIT_FAILURE);
        }
        try {
            server.sin_port = std::stoul(port);
        } catch (std::invalid_argument& e) {
            printf("Server's port is invalid\n");
            exit(EXIT_FAILURE);
        }
    }

    bool send(int sockfd, char* message, ssize_t size) {
        ssize_t count = 0;
        while (count < size) {
            ssize_t sent = ::send(sockfd, message + count, size - count, 0);
            if (sent == -1) {
                perror("Error during sending");
                return false;
            }
            count += sent;
        }
        return true;
    }

    bool recv(int sockfd, char* message, ssize_t size) {
        ssize_t count = 0;
        while (count < size) {
            ssize_t read = ::recv(sockfd, message, size, 0);
            if (read == -1 || read == 0) {
                return false;
            }
            count += read;
        }
        return true;
    }

    void close(int sockfd) {
        while (::close(sockfd) == -1) {
            perror("Cannot close socket, trying again");
        }
    }
}
