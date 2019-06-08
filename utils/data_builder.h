#ifndef MY_UTILS
#define MY_UTILS

#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>


struct data_builder {
    data_builder() = default;

    void append(std::string const &other);

    std::pair<int, std::string> process(int const &fd, size_t BF_SZ);

    std::string get_full_message();

private:
    std::string data;
};

#endif // MY_UTILS
