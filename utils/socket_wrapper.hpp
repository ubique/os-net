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

    int get_descriptor() const;

    bool check_valid() const;

    void close();

private:

    int descriptor;

};

#endif // SOCKET_WRAPPER_HPP
