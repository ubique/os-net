#ifndef SERVER_H
#define SERVER_H

#include "utils/socket_wrapper.hpp"

#include <netinet/in.h>

class tftp_server {

public:

    tftp_server();

    ~tftp_server();

    [[ noreturn ]] void await();

    void respond();

private:

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // SERVER_H
