#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#include "logger.hpp"

#include <unistd.h>
#include <sys/socket.h>

class socket_wrapper {

public:

    socket_wrapper();

    socket_wrapper(int descriptor);

    ~socket_wrapper();

    void renew();

    int get_descriptor() const;

    bool check_valid() const;

    int send_message(char const *buf, size_t buffer_size, unsigned int repeat = 1, bool log_success = true);

    int receive_message(char *buf, size_t &buffer_size,
                        unsigned int real_size, unsigned int repeat = 1, bool log_success = true);

    void close();

private:

    int descriptor;

};

#endif // SOCKET_WRAPPER_HPP
