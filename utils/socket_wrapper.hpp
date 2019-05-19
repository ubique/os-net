#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#include "utils.hpp"

#include <unistd.h>
#include <sys/socket.h>

class socket_wrapper {

public:

    socket_wrapper() : descriptor(socket(AF_INET, SOCK_STREAM, 0)) {};
    socket_wrapper(int descriptor) : descriptor(descriptor) {};
    ~socket_wrapper() = default;

    int &get_descriptor() {
        return descriptor;
    }
    bool check_valid() const {
        return descriptor != -1;
    }

    void close() {
        if (close(descriptor)) {
            console::report("Could not close the socket properly", errno)
        }
    }

private:

    int descriptor;

};

#endif // SOCKET_WRAPPER_HPP
