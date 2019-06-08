//
// Created by Михаил Терентьев on 2019-06-08.
//
#include "data_builder.h"

void printError(const std::string &message) {
    std::cerr << message << std::endl;
}

void println(const std::string &message) {
    std::cout << message << std::endl;
}

void print(const std::string &message) {
    std::cout << message;
    std::cout.flush();
}


void data_builder::append(std::string const &other) {
    data += other;
}

std::pair<int, std::string> data_builder::process(int const &fd, size_t const BF_SZ) {
    int stat = 0;
    std::string t;
    while (true) {
        std::string cur;
        t = get_full_message();
        if (!t.empty()) {
            break;
        }
        std::vector<char> bf(BF_SZ);
        ssize_t prs_sz = read(fd, bf.data(), BF_SZ);
        if (prs_sz == -1) {
            return {-1, ""};
        }
        stat += prs_sz;
        bf.resize(static_cast<size_t>(prs_sz));
        bf.push_back('\0');
        cur = std::string(bf.data());
        if (cur.empty()) {
            break;
        }
        append(cur);
    }
    return {stat, t};
}

std::string data_builder::get_full_message() {
    size_t end = data.find("\r\n");
    if (end == std::string::npos) {
        return "";
    }
    std::string res = data.substr(0, end);
    data = data.substr(end + 2);
    return res;
}