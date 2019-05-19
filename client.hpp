#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <cstdlib>

#include "utils/socket_wrapper.hpp"

#include <sys/types.h>
#include <sys/socket.h>

class tftp_client {

public:

    tftp_client(std::string const &address, size_t server_port);
    ~tftp_client();

    bool send_file(std::string const &path);
    bool request_file(std::string const &path);

    void close();

private:

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // CLIENT_HPP
