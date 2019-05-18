//
// Created by utusi on 16.05.19.
//

#include <DBase.h>

DBase::DBase() {
    users["test"] = User("test", "test");
    users["oleg"] = User("oleg", "password");
    messages["test"].push_back(Message("test", "text1 qwe"));
    messages["test"].push_back(Message("test", "text232 rtt"));
    messages["oleg"].push_back(Message("oleg", "hello os"));
    messages["oleg"].push_back(Message("oleg", "itmo course"));
    messages["oleg"].push_back(Message("oleg", "gamest"));
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

void DBase::remove_msg(const std::string &login, int number) {
    if (number <= 0) {
        return;
    }
    if (messages.count(login) == 0) {
        return;
    }
    if (messages[login].size() < number) {
        return;
    }
    messages[login][number - 1].remove();
}

void DBase::unremove_msgs(const std::string &login) {
    if (messages.count(login) == 0) {
        return;
    }
    for(auto &i : messages[login]) {
        i.unremove();
    }
}

