//
// Created by vitalya on 19.05.19.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

class server {
public:
    server() = default;
    server(char* socket_name);
    ~server();

    server(const server& other) = delete;
    server& operator=(const server& other) = delete;

    void start();

private:
    struct sockaddr_un address;
    int connection_socket = -1;
    bool down_flag = false;
    std::string sock_name;

    static const size_t BUFFER_SIZE;
};


#endif //OS_NET_SERVER_H
