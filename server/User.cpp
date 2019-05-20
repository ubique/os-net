//
// Created by Кирилл Чулков on 2019-05-11.
//

#include <utility>

#include "User.h"

User::User(std::string  name, std::string  password) : name(std::move(name)), password(std::move(password)) {
    messages = std::vector<Message>(0);
}

void User::delMessage(int index) {
    messages[index].deleted();
}

std::string User::getName() {
    return name;
}

bool User::validatePassword(const std::string& password) {
    return this->password == password;
}

size_t User::getSizeOf(int index) {
    return messages[index].getSize();
}

std::string& User::getMessage(int index) {
    return messages[index].getText();
}

size_t User::validateIndex(size_t index) {
    int j = 0;
    for (int i = 0; i < messages.size(); ++i) {
        if (!messages[i].isDeleted()) {
            ++j;
        }
        if (j == index) {
            return i;
        }
    }
    return -1;
}

void User::addMessage(const std::string& text) {
    messages.emplace_back(text);
}
