#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static const int BUF_SIZE = 4096;
static in_port_t PORT = 7;
static std::string msg = "Hello, world!";

void ext(const char *msg, int socket) {
    std::cerr << msg << strerror(errno) << "\n";
    close(socket);
    exit(EXIT_FAILURE);
}

bool send(int sock_fd, const std::string &msg) {
    ssize_t size = static_cast<ssize_t>(msg.size());
    ssize_t was_sent = 0, left = size;

    int tries = 5;

    while (left > 0 && tries > 0) {
        ssize_t sent = send(sock_fd, msg.data() + was_sent, left, 0);
        if (sent == -1) {
            return false;
        }
        if (sent == 0) {
            --tries;
        }

        was_sent += sent;
        left -= sent;
    }
    return was_sent == size;
}

bool get_response(int sock_fd, std::string &response) {
    char buffer[BUF_SIZE];
    bzero(buffer, BUF_SIZE);

    ssize_t r = read(sock_fd, buffer, BUF_SIZE);
    if (r == -1) {
         return false;
    }
    for (ssize_t i = 0; i < r; ++i) {
        response += (char) buffer[i];
     }

    return true;
}

int main(int argc, char* argv[]) {
        if (argc < 2) {
            std::cerr << "Usage: ./client address [msg] \n";
            return 0;
        }
        in_addr_t address = inet_addr(argv[1]);

        if (argc == 3) {
            msg = std::move(argv[2]);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = PORT;
        addr.sin_addr.s_addr = address;

        int sock_fd = socket(addr.sin_family, SOCK_STREAM , 0);
        if (sock_fd == -1) {
            ext("socket() failed: ", sock_fd);
        }

        if (connect(sock_fd, reinterpret_cast<sockaddr *>(&addr), sizeof (addr)) == -1) {
            ext("connect() failed: ", sock_fd);
        }

        if (!send(sock_fd, msg)) {
            ext("can't send message: ", sock_fd);
        };
        std::string response = "Server: \n";

        if (!get_response(sock_fd, response)) {
            ext("get_response failed: ", sock_fd);
        }
        std::cout << response;

        close(sock_fd);
        return 0;

}
