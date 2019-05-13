//
// Created by rsbat on 5/12/19.
//

#ifndef OS_NET_POP_SERVER_H
#define OS_NET_POP_SERVER_H

#include "fd_wrapper.h"

#include <string>

class POP_server {
public:
    POP_server(std::string address, std::string port);

    [[noreturn]] void run() ;
private:
    enum class State { WAIT, AUTH, TRANSACTION, UPDATE };
    static constexpr int MAX_BACKLOG = 50;
    Fd_wrapper socket_fd;
    State state;
    std::string current_user;
};


#endif //OS_NET_POP_SERVER_H
