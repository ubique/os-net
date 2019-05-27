//
// Created by dumpling on 27.05.19.
//

#ifndef FD_WRAPPER_H
#define FD_WRAPPER_H

#include <string>
#include <unistd.h>

void print_err(const std::string &);

struct fd_wrapper {

    fd_wrapper(int fd) : fd(fd) {}

    fd_wrapper(const fd_wrapper &other) = delete;

    fd_wrapper(fd_wrapper &&other) {
        fd = other.fd;
        other.fd = -1;
    }

    int get() const {
        return fd;
    }

    ~fd_wrapper() {
        if (fd != -1 && close(fd) == -1) {
            print_err("Error while close fd");
        }
    }

private:
    int fd;
};

#endif //FD_WRAPPER_H
