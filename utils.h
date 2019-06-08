#ifndef UTILS_H
#define UTILS_H 

#include <arpa/inet.h>
#include <string>

namespace utils {
    const size_t BUF_SIZE = 256;
    const size_t BACKLOG = 10;

    void init(int& sockfd, sockaddr_in& server, const std::string& addr, const std::string& port);
    void bind_and_listen(int& sockfd, sockaddr_in& server);
    void connect(int& sockfd, sockaddr_in& server);

    bool send(int sockfd, char* message, ssize_t size);
    bool recv(int sockfd, char* message, ssize_t size);

    void close(int sockfd);
}

#endif
