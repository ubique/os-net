//
// Created by utusi on 11.05.19.
//

#include "User.h"

std::string User::get_login() {
    return login;
}

bool User::cmp_password(const std::string &password) {
    return this->password == password;
}

User::User() : login(""), password("")
{
}

User::User(const std::string &login, const std::string &password): login(login), password(password)
{
}
