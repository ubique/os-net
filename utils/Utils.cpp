//
// Created by ifkbhit on 07.05.19.
//

#include <iostream>
#include <sstream>
#include "Utils.h"
#include "../protocol/ECHOProtocol.h"

bool Utils::error(const std::string& message) {
    std::cerr << message << std::endl;
    return false;
}

void Utils::message(const std::string& message) {
    std::cout << message << std::endl;

}

bool Utils::fail(const std::string& message) {
    perror(message.c_str());
    return false;
}


std::map<std::string, std::string> Utils::argParse(bool forClient, int argc, char** argv) {

    std::map<std::string, std::string> args = {
            {"a", Utils::DEFAULT_IP},
            {"p", Utils::DEFAULT_PORT},
    };
    if (forClient) {
        args.insert({"d", Utils::DEFAULT_DATA});
    }

    for (int i = 2; i < argc; i += 2) {
        std::string param = std::string(argv[i]);
        if (param[0] == '-') {
            param = param.substr(1);
        } else {
            error("Wrong parameter '" + param + "'");
            return {};
        }
        if (i == argc - 1) {
            error("Wrong arguments count");
            return {};
        }
        std::string value = std::string(argv[i + 1]);
        if (args.count(param) == 0) {
            std::cerr << "Undefined argument '" << param << "'" << std::endl;
            return {};
        }
        args[param] = value;

    }
    return args;
}

const std::string Utils::DEFAULT_IP = "127.0.0.1";
const std::string Utils::DEFAULT_PORT = "10567";
const std::string Utils::DEFAULT_DATA = DEFAULT_IP;



void Utils::help() {
    Utils::message("OS-NET HW implementation. Usage:\n\n"
                   "      (executable) server [arguments... ]\n"
                   "      or\n"
                   "      (executable) client [arguments... ]\n");

    Utils::message("Simple tcp client. Usage client: "
                   "client\n"
                   "      [-a ipv4 address(default 127.0.0.1)]\n"
                   "      [-p port(default 10567)]\n"
                   "      [-d data to send (default '127.0.0.1')]\n"
    );

    Utils::message("Simple tcp server. Usage client: "
                   "server\n"
                   "      [-a ipv4 address(default 127.0.0.1)]\n"
                   "      [-p port(default 10567)]\n"
    );
    Utils::message("For example:");
    Utils::message("First call:\n"
                   "    (executable) server\n"
                   "then\n"
                   "    (executable) client\n"
                   "\n"
                   "First call run server on 127.0.0.1:10567\n"
                   "Second call send message '127.0.0.1' to server at 127.0.0.1:10567\n");
}

