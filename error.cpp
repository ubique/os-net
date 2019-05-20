//
// Created by roman on 20.05.19.
//

#include "error.h"

#include <cstring>
#include <stdexcept>

void error(std::string msg) {
    throw std::runtime_error(msg + ": " + strerror(errno));
}

