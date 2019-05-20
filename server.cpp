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

void print_err(const std::string &);

uint16_t get_port(const std::string &);

void send_all(int, const char *, int);

void print_help() {
    std::cout << "Usage: ./server [port] [address]" << std::endl;
}

int start_server(std::string &address, uint16_t port) {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        throw std::runtime_error("Can't create socket");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (bind(sfd, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Bind failed");
    }

    return sfd;
}

void echo(int reader) {
    char buf[BUF_SIZE];
    int bytes_read;

    while (true) {
        bytes_read = recv(reader, buf, BUF_SIZE, 0);

        if (bytes_read == -1) {
            throw std::runtime_error("Recv failed");
        }

        if (bytes_read == 0) {
            break;
        }

        send_all(reader, buf, bytes_read);
    }
}


void wait_for_connections(int listener) {

    if (listen(listener, LISTEN_BACKLOG) == -1) {
        throw std::runtime_error("Listen failed");
    }

    while (true) {
        int reader = accept(listener, nullptr, nullptr);

        if (reader == -1) {
            throw std::runtime_error("Accept failed");
        }

        int pid = fork();
        if (pid == -1) {
            throw std::runtime_error("Fork failed");
        }

        if (pid == 0) {
            try {
                echo(reader);
            } catch (std::runtime_error &e) {
                print_err(e.what());
            }
            close(reader);
            exit(EXIT_SUCCESS);
        } else {
            close(reader);
        }
    }
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


    int listener = 0;
    try {
        listener = start_server(address, port);
        wait_for_connections(listener);
        close(listener);
    } catch (std::runtime_error &e) {
        print_err(e.what());
        close(listener);
        return EXIT_FAILURE;
    }
}

