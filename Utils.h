//
// Created by Павел Пономарев on 2019-05-19.
//

#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

void printErrorMessage(std::string const& message, bool error = true) {
    std::cout << "Error occurred: " << message << ". ";
    if (error) {
        std::cout << std::strerror(errno);
    }
    std::cout << std::endl;
}

int parseInt(std::string const& str) {
    int value;
    std::istringstream stream(str);
    stream >> value;
    return value;
}

#endif //OS_NET_UTILS_H
