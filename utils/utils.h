#ifndef MY_UTILS
#define MY_UTILS

#include <string>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct fd_wrapper {
    fd_wrapper(int fd);
    fd_wrapper(fd_wrapper const&) = delete;
    fd_wrapper& operator=(fd_wrapper const&) = delete;
    bool bad();
    int const& value();
    ~fd_wrapper();

  private:
    int fd;
};

struct accumulator {
    accumulator() = default;

    std::string next();

    void insert(std::string const& n);

    std::pair<int, std::string> read_fully(int const& fd,
                                           size_t const BUFF_SIZE);

  private:
    std::string data;
};

#endif
