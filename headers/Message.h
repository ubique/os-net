//
// Created by utusi on 11.05.19.
//
#pragma once

#include <string>

class Message {
public:
    Message();
    Message(const std::string& login, const std::string &text);
    void set_text(const std::string &text);
    std::string get_text() const;

    std::string get_user_login() const;

private:
    std::string text;
    std::string user_login;
};