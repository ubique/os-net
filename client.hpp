#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <cstdlib>

#include "utils/socket_wrapper.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class client {

public:

    static unsigned int const REPEAT;

    client();

    ~client();

    void connect(std::string const &address, uint16_t port);

    void send(std::string const &message);

    void disconnect();

private:

    void receive();

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // CLIENT_HPP
