//
// Created by utusi on 16.05.19.
//

#include "Session.h"
#include <ctime>

void Session::update() {
    // update time
}

User Session::get_user() {
    return user;
}

bool Session::is_limit() {
    return false;
}

Session::Session() : user(), time(0)
{}

Session::Session(User user) : user(user), time(0)
{}
