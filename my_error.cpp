//
// Created by roman on 04.06.19.
//

#include "my_error.h"
#include <string>
#include <stdexcept>
#include <cstring>
#include <iostream>

void my_error(const std::string &msg) {
    std::cerr << ("[ERROR] " + msg + ": " + strerror(errno)) << std::endl;
}