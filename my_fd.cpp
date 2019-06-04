//
// Created by roman on 04.06.19.
//

#include "my_fd.h"
#include <unistd.h>
#include "my_error.h"

my_fd::my_fd(int fd) : fd(fd) {}

my_fd::~my_fd() {
    if (fd == -1) return;
    if (close(fd) == -1) {
        my_error("Error during closing file descriptor");
    }
}

my_fd::operator int() const {
    return fd;
}

my_fd::my_fd() : fd(-1) {
}
