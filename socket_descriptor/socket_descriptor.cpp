//
// Created by Anton Shelepov on 2019-05-17.
//

#include "socket_descriptor.h"
#include <sys/socket.h>
#include <unistd.h>

socket_descriptor::socket_descriptor() {
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
}

socket_descriptor::socket_descriptor(int descriptor) : descriptor(descriptor) {}

socket_descriptor::~socket_descriptor() {
    close(descriptor);
}

int socket_descriptor::operator*() const {
    return descriptor;
}

bool socket_descriptor::valid() const {
    return descriptor != -1;
}