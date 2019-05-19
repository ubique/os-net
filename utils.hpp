//
// Created by domonion on 02.05.19.
//

#include <string>

#ifndef FIND_UTILS_HPP
#define FIND_UTILS_HPP

void check_error(int rc, const std::string& additional);
void doSend(char * what, int amount, int where);
void doRecv(char * where, int amount, int from);
int const MAX_MESSAGE_LEN = 100;
int const SERVER_PORT = 8880;

#endif //FIND_UTILS_HPP
