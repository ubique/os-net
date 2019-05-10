#ifndef MY_CLIENT
#define MY_CLIENT

#include <stdexcept>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct client_exception : std::runtime_error {
    client_exception(std::string cause);
};

struct fd_wrapper {
    fd_wrapper(int fd) : fd(fd) {}
    fd_wrapper(fd_wrapper const&) = delete;
    fd_wrapper& operator=(fd_wrapper const&) = delete;
    fd_wrapper& operator=(int fd) {
        this->fd = fd;
        return *this;
    }
    bool bad() { return fd == -1; }
    int const& value() { return fd; }
    ~fd_wrapper() { close(fd); }

  private:
    int fd;
};

struct client {
    client(char* address, uint16_t port);

    std::string request(std::string text);

  private:
    fd_wrapper socket_fd;
    sockaddr_in server_address;

    static constexpr size_t BUFFER_SIZE = 2048;
};

#endif // MY_CLIENT
