#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#include <string>

class server {
public:
    server(const std::string &address, const std::string &port);

    void wait_client();
    void accept_connection();
    void work();

private:
    void check_ipv4(const std::string &address);
    void create_socket();
    void set_sock_options(const std::string &port);
    void bind_to_address();
    void detach(int fd);

    void print_fatal_error(const std::string &err, bool perr = true);

    int server_fd;
    int new_socket;
    struct sockaddr_in server_address;
    int addrlen = sizeof(server_address);
};

#endif  // SERVER_H
