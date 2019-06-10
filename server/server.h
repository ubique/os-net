//
// Created by Anton Shelepov on 2019-05-17.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <exception>
#include <string>
#include "../socket_descriptor/socket_descriptor.h"
#include <vector>

class server {

public:
    server(std::string const& address, int port);
    void log(std::string const& msg);
    void handle_connection(socket_descriptor const& client_socket, sockaddr_in const& client_addr);
    std::string read(int desc);
    void send(int desc, std::string const& message);
    void run();

private:
    socket_descriptor socket_fd = -1;
    sockaddr_in addr = {0};

    const int BACKLOG_QUEUE_SIZE = 5;
    const size_t BUFFER_SIZE = 4096;
    const size_t TRIES_NUMBER = 10;
    bool alive;
};


struct server_exception : std::runtime_error {
    server_exception(std::string const& msg);
};


#endif //OS_NET_SERVER_H
