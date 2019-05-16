//
// Created by utusi on 16.05.19.
//
#pragma once
#include "User.h"



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