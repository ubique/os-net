//
// Created by roman on 20.05.19.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H


#include <netinet/in.h>
#include <string>
#include "my_fd.h"

using std::string;

class client {
public:
    client();

    void connect_to(const char *hostAddress, const in_port_t port);
    string request(const string& message);

private:
    void receive_response();
    void do_request(const string& msg);
private:
    my_fd socket_fd;
    static constexpr size_t BUFFER_SIZE = 4096 * 2;
    char buffer[BUFFER_SIZE]; //for null terminated string
};


#endif //OS_NET_CLIENT_H
