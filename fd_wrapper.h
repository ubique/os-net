//
// Created by dumpling on 27.05.19.
//

#ifndef FD_WRAPPER_H
#define FD_WRAPPER_H

void print_err(const std::string &);

struct fd_wrapper {

    fd_wrapper(int fd) : fd(fd) {}

    int get() {
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
