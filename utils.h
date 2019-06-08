#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int closeSocket(int socket) {
    if (close(socket) == -1) {
        perror("close");
        return 1;
    }
    return 0;
}

int createSocket() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void setSocketOptions(int &socket) {
    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

bool recvAll (int socket, char* data, size_t size) {
    size_t total = 0;
    size_t bytes = 0;
    while (bytes < size) {
        bytes = recv(socket, data + total, size - total, MSG_WAITALL);
        if (bytes == -1)
            return true;
        total += bytes;
    }
    return false;
}

int* getSize (int socket) {
    size_t total = 0;
    size_t bytes = 0;
    int *data = nullptr;
    size_t size = sizeof(data);
    while (bytes < size) {
        bytes = recv(socket, data + total, size - total, MSG_WAITALL);
        if (bytes == -1)
            return nullptr;
        total += bytes;
    }
    return data;
}

bool sendMsg(int socket, const char* data, size_t size){
    size_t total  = 0;
    size_t bytes = 0;
    while(total < size){
        bytes = send(socket, data + total, size - total, 0);
        if(bytes == -1){
            return true;
        }
        total += bytes;
    }
    return false;
}

std::pair<socklen_t, sockaddr_in> genStructFields(int addr, int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = addr;
    return {sizeof(address), address};
}

#endif //OS_NET_UTILS_H
