//
// Created by Anton Shelepov on 2019-05-17.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include "../socket_descriptor/socket_descriptor.h"
#include <string>
#include <exception>
#include <netdb.h>

class client {
public:
    client();
    void establish_connection(std::string const& address, int port);
    std::string request(std::string const& message);
    std::string read(int desc);
    void send(int desc, std::string const& message);
    void log(std::string const& msg);
    void run();

private:
    socket_descriptor socket_fd = -1;
    sockaddr_in server_addr = {0};

    const size_t BUFFER_SIZE = 4096;
    const size_t TRIES_NUMBER = 10;
};

struct client_exception : std::runtime_error {
    client_exception(std::string const& msg);
};


#endif //OS_NET_CLIENT_H
