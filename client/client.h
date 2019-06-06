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

#include "utils/utils.h"

struct client_exception : std::runtime_error {
    client_exception(std::string cause);
};

struct client {
    client(char* address, uint16_t port);

    std::string request(std::string text, accumulator& acc);

  private:
    fd_wrapper socket_fd;
    sockaddr_in server_address;

    static constexpr size_t BUFFER_SIZE = 2048;
};

#endif // MY_CLIENT
