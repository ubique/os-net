//
// Created by utusi on 11.05.19.
//
#pragma once

#include <string>

class Message {
public:
    Message();
    Message(std::string  login, std::string text);
    void set_text(const std::string &text);
    std::string get_text() const;
    std::string get_user_login() const;
    bool is_removed();
    void remove();
    void unremove();
    size_t get_size();
private:
    std::string text;
    std::string user_login;
    bool removed{};
};