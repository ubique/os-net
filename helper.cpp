//
// Created by max on 01.05.19.
//

#include <cstring>
#include "helper.h"

void prerror(const char *msg) {
    std::cerr << msg << std::endl;
    std::cerr << strerror(errno) << std::endl;
}

std::string prerror_str(const char *msg) {
    std::string ans;
    ans += msg;
    ans += '\n';
    ans += strerror(errno);
    ans += '\n';
    return ans;
}