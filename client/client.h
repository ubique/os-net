//
// Created by vitalya on 19.05.19.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <string>
#include <sys/socket.h>
#include <sys/un.h>

class client {
public:
    client() = default;
    client(char* socket_name);
    ~client() = default;

    std::string sendAndReceive(const std::string &message);

private:
    struct sockaddr_un address;
    static const size_t BUFFER_SIZE;
};

#endif //OS_NET_CLIENT_H
