//
// Created by dumpling on 30.04.19.
//

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

static const int LISTEN_BACKLOG = 50;
static const int BUF_SIZE = 4096;


int start(std::string &address, uint16_t port) {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (connect(sfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        return -1;
    }

    return sfd;
}

int send_and_receive(int sender, int listener, std::string &message) {

    auto m = message.c_str();
    int size = message.length();
    int total = 0;

    while (total < size) {
        int was_send = send(sender, m + total, size - total, 0);
        if (was_send == -1) {
            return -1;
        }

        total += was_send;
    }

    close(sender);

    char buf[BUF_SIZE];
    int bytes_read;
    while (true) {
        bytes_read = recv(listener, buf, BUF_SIZE, 0);

        if (bytes_read == -1) {
            break;
        }

        if (bytes_read == 0) {
            break;
        }

        for (int i = 0; i < bytes_read; ++i) {
            std::cout << buf[i];
        }
    }

    close(listener);

    std::cout << std::endl;

    return true;
}


int main(int argc, char **argv) {

    std::string address = "127.0.0.1";
    uint16_t port = 8005;


    std::string message;
    while (getline(std::cin, message)) {
        int sender = start(address, port);
        int listener = start(address, port);
        send_and_receive(sender, listener, message);
    }

}

