//
// Created by utusi on 16.05.19.
//

#include <DBase.h>

DBase::DBase() {

}

bool DBase::is_user_by_login(const std::string &login) const {
    return users.count(login) > 0;
}

std::vector<Message> DBase::get_messages_by_login(const std::string &login) {
    std::vector<Message> result;
    if (messages.count(login) == 0) {
        return result;
    }
    for(Message msg : messages[login]) {
        if (!msg.is_removed()) {
            result.push_back(msg);
        }
    }
    return result;
}

User DBase::get_user_by_login(const std::string &login) {
    User user;
    if (users.count(login) == 0) {
        return user;
    }
    return users[login];
}

void DBase::update(const std::string &login) {
    if (messages.count(login) == 0) {
        return;
    }
    for(size_t i = 0; i < messages[login].size(); i++) {
        if (messages[login][i].is_removed()) {
            messages[login].erase(messages[login].begin() + i);
        }
    }
}

