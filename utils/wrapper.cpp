//
// Created by Михаил Терентьев on 2019-06-08.
//

#include "wrapper.h"

wrapper::wrapper() : descriptor(-1) {}

wrapper::wrapper(int const &fd) : descriptor(fd) {}

bool wrapper::isBroken() { return descriptor == -1; }

int wrapper::getDescriptor() { return descriptor; }

wrapper &wrapper::operator=(wrapper &&other) noexcept {
    std::swap(other.descriptor, descriptor);
    other.descriptor = -1;
    return *this;
}

wrapper::~wrapper() {
    if (descriptor != -1) {
        close(descriptor);
    }
}