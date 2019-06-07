#ifndef SERVER_H
#define SERVER_H

#include "utils/socket_wrapper.hpp"

#include <netinet/in.h>

class server {

public:

    static unsigned int const REPEAT;
    static unsigned int const MAX_QUEUE;
    static unsigned int const BUFFER_SIZE;

    server(std::string const &address, uint16_t port);

    ~server();

    [[ noreturn ]] void await_and_respond();

private:

    void respond(socket_wrapper &client_desc, char *buf, size_t buffer_size);

    socket_wrapper socket_desc;
    sockaddr_in server_address;

};

#endif // SERVER_H
