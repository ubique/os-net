//
// Created by Кирилл Чулков on 2019-05-11.
//

#ifndef OS_LIB_USER_H
#define OS_LIB_USER_H

#include <string>
#include <vector>
#include <cstddef>

#include "Message.h"

class User {
public:
    User() = default;
    User(std::string name, std::string password);

    std::string getName();
    bool validatePassword(const std::string& password);

    size_t validateIndex(size_t index);
    void delMessage(int index);
    size_t getSizeOf(int index);
    std::string& getMessage(int index);
    void addMessage(const std::string& text);

private:
    std::string name;
    std::string password;
    std::vector<Message> messages;
};


#endif //OS_LIB_USER_H
