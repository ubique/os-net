#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <stdio.h>

class POP3Server {
public:
    int run(const std::string& host_name, int port);
    void execute();
private:
    const int FAIL = 0;
    const int SUCCESS = 1;
    int socket_fd{};
    struct sockaddr_in server_addr{};

    void print_error(const std::string &msg);
};