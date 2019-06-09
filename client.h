#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H


#include <netinet/in.h>
#include <string>

class client {
public:
    client(uint32_t address, in_port_t port);

    ~client();

    client(const client &other) = delete;

    client &operator=(const client &other) = delete;

    void connect_to_server();

    void echo(std::string &str);

private:
    const uint32_t address;
    const in_port_t port;
    int sock;
};


#endif //OS_NET_CLIENT_H
