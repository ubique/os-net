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

void checker(int ret, const char *msg, int error_code) {
    if (ret == error_code) {
        std::string err_msg = prerror_str(msg);
        std::cerr << err_msg << std::endl;
        throw std::runtime_error(err_msg);
    }
}


void checker(int ret, const std::string &msg, int error_code) {
    checker(ret, msg.data(), error_code);
}

