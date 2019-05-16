//
// Created by utusi on 16.05.19.
//
#pragma once
#include <vector>
#include <map>
#include "User.h"
#include "Message.h"

class DBase {
public:
    DBase();
    bool is_user_by_login(const std::string &login) const;
    std::vector<Message> get_messages_by_login(const std::string &login);
    User get_user_by_login(const std::string &login);
private:
    std::map<std::string, std::vector<Message>> messages;
    std::map<std::string, User> users;
};
