#pragma once

#include <vector>
#include <string>
#include <map>
#include "../protocol/Protocol.h"

using data_t = std::vector<char>;

class Utils {

public:

    static const std::string DEFAULT_IP;
    static const std::string DEFAULT_PORT;
    static const std::string DEFAULT_DATA;


    /**
     * Support functions
     */

    static bool fail(const std::string& message);

    static bool error(const std::string& message);

    static void message(const std::string& message);


    static void help();

    static std::map<std::string, std::string> argParse(bool forClient, int argc, char** argv);

};


