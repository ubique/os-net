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

static const int BUF_SIZE = 4096;

void print_err(const std::string &);

uint16_t get_port(const std::string &);

void send_all(int, const char *, int);

void print_help() {
    std::cout << "Usage: ./client [port] [address]" << std::endl;
}

int open_connection(std::string &address, uint16_t port) {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        throw std::runtime_error("Can't create socket");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (connect(sfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::runtime_error("Can't connect to the server");
    }

    return sfd;
}

void send_and_receive(int sender, std::string &message) {

    auto m = message.c_str();
    int size = message.length();
    send_all(sender, m, size);

    char buf[BUF_SIZE];
    int bytes_read;
    int sum = 0;
    while (sum < size) {
        bytes_read = recv(sender, buf, BUF_SIZE, 0);

        if (bytes_read == -1) {
            throw std::runtime_error("Recv failed");
        }

        if (bytes_read == 0) {
            break;
        }

        for (int i = 0; i < bytes_read; ++i) {
            std::cout << buf[i];
        }
        sum += bytes_read;
    }

    std::cout << std::endl;
}


int main(int argc, char **argv) {

    std::string address = "127.0.0.1";
    uint16_t port = 3456;

    if (argc > 1) {

        if (std::string(argv[1]) == "help") {
            print_help();
            return EXIT_SUCCESS;
        }

        try {
            port = get_port(std::string(argv[1]));
        } catch (std::invalid_argument &e) {
            print_err(e.what());
            return EXIT_FAILURE;
        } catch (std::out_of_range &e) {
            print_err("Port is too big");
            return EXIT_FAILURE;
        }
    }

    if (argc > 2) {
        address = std::string(argv[2]);
    }

    if (argc > 3) {
        print_err("Wrong arguments, use help");
        return EXIT_FAILURE;
    }


    std::string message;
    while (getline(std::cin, message)) {
        try {
            int sender = open_connection(address, port);
            send_and_receive(sender, message);
            close(sender);
        } catch (std::runtime_error &e) {
            print_err(e.what());
            return EXIT_FAILURE;
        }
    }

}

