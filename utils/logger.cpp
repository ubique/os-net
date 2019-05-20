#include "logger.hpp"

logger::logger() : out(std::cerr) {}

logger::logger(std::ostream &out) : out(out) {}

int logger::fail(std::string const &message, int err) {
    out << _ERROR << message;
    if (err != 0) {
        out << ": " << std::strerror(errno);
    }
    out <<  _DEFAULT << std::endl;
    return 0;
}

void logger::success(std::string const &message) {
    out << _LOG << message << _DEFAULT << std::endl;
}