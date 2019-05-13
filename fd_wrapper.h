//
// Created by rsbat on 5/13/19.
//

#ifndef OS_NET_FD_WRAPPER_H
#define OS_NET_FD_WRAPPER_H

#include <unistd.h>
#include <iostream>
#include <cstring>

class Fd_wrapper {
public:
    Fd_wrapper() :fd(-1) {}
    explicit Fd_wrapper(int descriptor) : fd(descriptor) {}
    ~Fd_wrapper() {
        close();
    }

    void set(int descriptor) {
        close();
        fd = descriptor;
    }

    void close() {
        if (fd != -1) {
            if (::close(fd) == -1) {
                std::cerr << "Could not close file descriptor: " << strerror(errno) << std::endl;
            }
            fd = -1;
        }
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    operator int() {
        return fd;
    }
#pragma clang diagnostic pop

private:
    int fd;
};

#endif //OS_NET_FD_WRAPPER_H
