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
#include <vector>
#include <stdio.h>
#include "Message.h"

class POP3Server {
public:
    int run();
    POP3Server(const std::string& host_name, int port);
    ~POP3Server();
private:
    const std::string CRLF = "\r\n";
    const size_t BUFFER_LENGHT = 1024;
    enum States {AUTHORIZATION, TRANSACTION, UPDATE};
    int socket_fd{};
    struct sockaddr_in server_addr{};

    void print_error(const std::string &msg);
    void send_msg(const std::string& msg, int fd, const std::string& msg_error);
    size_t get_size_of_vector(std::vector<Message> & messages);
    void stop();
};