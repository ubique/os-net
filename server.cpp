#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

static const size_t BUF_SIZE = 4096;
static const int BACKLOG = 5;
static const std::string GREETING = "ECHO server ready\n";
static const in_port_t PORT = 7;
static const char *address = "127.0.0.1";

void ext(const char *msg, int socket) {
    std::cerr << msg << strerror(errno) << "\n";
    close(socket);
    exit(EXIT_FAILURE);
}

bool send(int sock_fd, char buffer[], ssize_t len) {
    ssize_t left = len, was_sent = 0;

    int tries = 5;

    while (left > 0 && tries > 0) {
        ssize_t sent = send(sock_fd, buffer + was_sent, left, 0);
        if (sent == -1) {
            return false;
        }
        if (sent == 0) {
            --tries;
        }

        was_sent += sent;
        left -= sent;
    }
    return was_sent == len;
}

int accept(int sock_fd) {
    struct sockaddr_in client_sock;
    socklen_t size;
    int fd = accept(sock_fd, reinterpret_cast<sockaddr *>(&client_sock), &size);
    if (fd == -1) {
        ext("accept() failed: ", sock_fd);
    }
    return fd;
}

int process_request(int fd) {
   char buffer[BUF_SIZE] ;
   memset(buffer, '\0', BUF_SIZE);
   ssize_t size = 0;

   while (true) {
       int got = read(fd, buffer, BUF_SIZE);
       if (got == 0) break;
       if (got == -1 || !send(fd, buffer, got)) {
            return -1;
       }
       size += got;
   }
   return size;
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        ext("socket() failed: ", sock_fd);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = PORT;
    //addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_addr.s_addr = inet_addr(address);
    if ((bind(sock_fd, reinterpret_cast<sockaddr *>(&addr), sizeof (addr))) == -1) {
        ext("bind() failed: ", sock_fd);
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        ext("listen() failed: ", sock_fd);
    }

    std::cout << GREETING;

    for (int i = 0; i < BACKLOG; ++i) {
        int cliend_fd = accept(sock_fd);
        if (process_request(cliend_fd) == -1) {
            close(cliend_fd);
            ext("process_request() failed: ", sock_fd);
        };
        close(cliend_fd);
    }

    close(sock_fd);
    std::cout << "\n For today enough!\n";
    return 0;
}
