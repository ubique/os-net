#ifndef UTILS_HPP
#define UTILS_HPP

class console {

public:

    static std::string const _ERROR = "\033[31;1m";
    static std::string const _HELP = "\033[32;1m";
    static std::string const _DEFAULT = "\033[0m";
    static std::string const _BOLD = "\033[1m";
    static std::string const _EMPHASIZE = "\033[33;1m";

    static int report(std::string const &message, int err = 0) {
        std::cerr << _ERROR << message;
        if (err != 0) {
            std::cerr << ": " << std::strerror(errno);
        }
        std::cerr << std::endl << _DEFAULT;
        return 0;
    }

}

#endif // UTILS_HPP
