//
// Created by rsbat on 5/12/19.
//

#ifndef OS_NET_POP_SERVER_H
#define OS_NET_POP_SERVER_H

static const int BUFFER_SIZE = 512;

#include "fd_wrapper.h"
#include "Mail_DB.h"

#include <string>

class POP_server {
public:
    POP_server(std::string address, std::string port);

    [[noreturn]] void run() ;
private:
    enum class State { WAIT, AUTH, TRANSACTION, UPDATE };
    static constexpr int MAX_BACKLOG = 50;
    Mail_DB mail_db;
    Fd_wrapper socket_fd;
    Fd_wrapper client_socket;
    State state;
    std::string current_user;

    void send_OK(std::string const& message);
    void send_OK();
    void send_ERR(std::string const& message);
    void process_command(std::string command);
    void send_all(const char* data, size_t size);
};


#endif //OS_NET_POP_SERVER_H
