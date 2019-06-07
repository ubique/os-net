#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cstring>
#include <string>

class logger {

public:

    std::string const _ERROR = "\033[31;1m";
    std::string const _HELP = "\033[32;1m";
    std::string const _LOG = "\033[34;1m";
    std::string const _DEFAULT = "\033[0m";

    logger();

    logger(std::ostream &out);

    int fail(std::string const &message, int err = 0);

    void success(std::string const &message);

private:

    std::ostream &out;

};

#endif // UTILS_HPP
