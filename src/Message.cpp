#include <utility>

#include <utility>

//
// Created by utusi on 11.05.19.
//


#include "Message.h"

Message::Message():
    user_login(""),
    text(""),
    removed(false)
{}

Message::Message(std::string login, std::string text):
    user_login(std::move(login)),
    text(std::move(text)),
    removed(false)
{}

void Message::set_text(const std::string &text) {
    this->text = text;
}

std::string Message::get_text() const {
    return text;
}

std::string Message::get_user_login() const {
    return user_login;
}

bool Message::is_removed() {
    return removed;
}

void Message::remove() {
    this->removed = true;
}

size_t Message::get_size() {
    return text.size();
}

void Message::unremove() {
    if (this->removed) {
        this->removed = false;
    }
}
