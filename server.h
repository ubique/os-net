#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H


#include <netinet/in.h>

class server {
public:
    server(in_port_t port);

    ~server();

    server(const server &other) = delete;

    server &operator=(const server &other) = delete;

    void start();

private:
    const in_port_t port;
    int sock;
};


#endif //OS_NET_SERVER_H
