//
// Created by utusi on 16.05.19.
//

#include "User.h"

#ifndef CLIENT_SERVER_SESSION_H
#define CLIENT_SERVER_SESSION_H

#endif //CLIENT_SERVER_SESSION_H


class Session {
public:
    Session();
    Session(User user);
    void update();
    User get_user();
    bool is_limit();
private:
    const time_t LIMIT = 10 * 60;
    User user;
    time_t time;
};