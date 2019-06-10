//
// Created by Anton Shelepov on 2019-06-10.
//

#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <string>
#include <cstddef>

namespace utils {
    std::string read(int desc, size_t expected = 0);
    size_t send(int desc, std::string const& message);
    const size_t BUFFER_SIZE = 10 * 4096;
    const size_t TRIES_NUMBER = 10;
}

#endif //OS_NET_UTILS_H
