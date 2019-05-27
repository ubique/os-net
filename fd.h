#include <iostream>
#include <unistd.h>
#include <fcntl.h>

struct fd_wrapper {
    explicit fd_wrapper(char const *file_name) {
        fd = open(file_name, O_RDONLY);
    }

    explicit fd_wrapper(int fd) : fd(fd) {}

    fd_wrapper(fd_wrapper const&) = delete;
    
    int get_fd() { return fd; }
    
    ~fd_wrapper() { 
        if (fd != -1) {
            if (close(fd) == -1) {
                std::cerr << "Can't close fd" << std::endl;
            }
        }
    }

  private:
    int fd;
};
