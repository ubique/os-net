//
// Created by utusi on 11.05.19.
//


#include "Message.h"

Message::Message(): user_login(""), text("")
{
}

Message::Message(const std::string &login, const std::string &text): user_login(login), text(text)
{
}

void Message::set_text(const std::string &text) {
    this->text = text;
}

std::string Message::get_text() const {
    return text;
}

std::string Message::get_user_login() const {
    return user_login;
}
