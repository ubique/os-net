//
// Created by utusi on 16.05.19.
//
#include "Utils.h"
#include <sstream>

std::vector<std::string> Utils::split(const std::string& str) {
    std::vector<std::string> tokens;
    std::stringstream stream;
    stream << str;
    std::string tmp;
    while(stream >> tmp) {
        tokens.push_back(tmp);
    }
    return tokens;
}
