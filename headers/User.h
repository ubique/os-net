//
// Created by utusi on 11.05.19.
//
#pragma once
#include <string>

class User {
public:
    User();
    User(std::string login, std::string password);
    std::string get_login();
    bool cmp_password(const std::string &password);
private:
    std::string login;
    std::string password;
};
