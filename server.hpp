#ifndef SERVER_H
#define SERVER_H

#include "utils/socket_wrapper.hpp"

#include <netinet/in.h>

class server {

public:

    server(std::string const &address, uint16_t port);

    ~server();

    [[ noreturn ]] void await_and_respond();

private:

    void respond(sockaddr_in &client_address, char *buf, ssize_t n, socklen_t len);

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // SERVER_H
