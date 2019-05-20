//
// Created by Кирилл Чулков on 2019-05-11.
//

#ifndef OS_NET_POP3SERVER_H
#define OS_NET_POP3SERVER_H

#include <string>
#include <vector>
#include <cstddef>

#include "User.h"

enum ServerState {
    AUTHORIZATION,
    TRANSACTION,
    UPDATE
};

class POP3Server {
public:
    explicit POP3Server(std::vector<User> users = std::vector<User>(0), ServerState state = ServerState::AUTHORIZATION);

    std::string processCommand(char* buf);

private:
    std::string userName = "noName";
    std::vector<User> users;
    ServerState state;
};


#endif //OS_NET_POP3SERVER_H
