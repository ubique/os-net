//
// Created by rsbat on 5/12/19.
//

#ifndef OS_NET_POP_SERVER_H
#define OS_NET_POP_SERVER_H

static const int BUFFER_SIZE = 512;

#include "fd_wrapper.h"
#include "Mail_DB.h"

#include <string>
#include <array>

class POP_server {
public:
    POP_server(const std::string& address, const std::string& port);

    [[noreturn]] void run() ;
private:
    enum class State { WAIT, AUTH, TRANSACTION, UPDATE };
    static constexpr int MAX_BACKLOG = 50;
    Mail_DB mail_db;
    Fd_wrapper socket_fd;
    Fd_wrapper client_socket;
    State state;
    std::string current_user;
    char buffer[BUFFER_SIZE];

    void send_OK(std::string const& message);
    void send_OK();
    void send_ERR(std::string const& message);
    void process_command();
    void send_all(const char* data, size_t size);
    std::string receive_command();
};


#endif //OS_NET_POP_SERVER_H
