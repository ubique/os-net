#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>

#include <cstddef>
#include <string>

const size_t buffer_size = 1024;

class client {
public:
    client(const std::string &address, const std::string &port);

    void connect_to_server();
    void work(const std::string &msg);

private:
    void create_socket();
    void set_sock_options(const std::string &addr_to_connect,
                          const std::string &port);
    void detach(int fd);

    void print_fatal_error(const std::string &err, bool perr = true);

    int client_fd = 0;
    struct sockaddr_in serv_addr;
};

#endif  // CLIENT_H
