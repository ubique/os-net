//
// Created by Кирилл Чулков on 2019-05-11.
//

#include <utility>
#include <stdio.h>
#include <sstream>
#include <iostream>

#include "POP3Server.h"

POP3Server::POP3Server(std::vector<User> users, ServerState state) : users(std::move(users)), state(state) {
    users.emplace_back("root", "root");
    users[0].addMessage("I am root");
    users[0].addMessage("My second mail");
    users[0].addMessage("0123456789");
    users.emplace_back("toor", "toor");
    users[1].addMessage("I am toor");
    users[1].addMessage("Toor's second mail");
    users[1].addMessage("9876543210");
}

std::string POP3Server::processCommand(char* buf) {
    char command[1024];
    std::stringstream ss;
    ss.str(buf);
    std::string name;
    int i;
    ss >> name;
    ss >> i;
    std::cout << name << " " << i;
    printf("%s %i", name.c_str(), i);
    return "Name " + this->userName + "\n";
}


