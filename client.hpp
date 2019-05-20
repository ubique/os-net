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

    client();

    ~client();

    void connect(std::string const &address, uint16_t port);

    void send(std::string const &local_path, std::string const &remote_path);

    void receive(std::string const &path, std::string const &remote_path);

    void disconnect();

private:

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // CLIENT_HPP
