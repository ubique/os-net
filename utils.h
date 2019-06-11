#pragma once

#include <string>
#include <unistd.h>


void print_error(std::string const& message);
bool check_port(const char* port);
void send_all(const char* pointer, size_t len, int descriptor);
std::string recv_all(int descriptor, char* buffer, size_t buffer_size);

class descriptor_wrapper {
    int descriptor;
public:
    descriptor_wrapper(int fd) : descriptor(fd) {}
    ~descriptor_wrapper() {
        if (descriptor != -1 && close(descriptor) == -1) {
            print_error("Can't close descriptor: ");
        }
        descriptor = -1;
    }
    operator int() const {
        return descriptor;
    }
    descriptor_wrapper& operator=(int fd) {
        descriptor = fd;
        return *this;
    }
};


