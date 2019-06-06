#include "rwutil.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static std::vector<std::string> parse_string(std::string const &target)
{
    std::istringstream stream(target);
    std::vector<std::string> result;
    while (stream) {
        std::string word;
        stream >> word;
        result.push_back(word);
    }
    return result;
}

static const size_t BUFFER_SIZE = 4096;

struct client_state {
    ssize_t sending = 0;
    std::string send_message = "";
};

struct fd_wrapper {
    int fd;
    explicit fd_wrapper(int fd) : fd(fd) {}
    fd_wrapper(fd_wrapper const &) = delete;
    ~fd_wrapper()
    {
        if (fd != -1 && close(fd) == -1) {
            std::cerr << "Error while closing fd: " << strerror(errno)
                      << std::endl;
        }
    }
};

void run(std::string const &address, std::string const &port)
{
    struct sockaddr_in server;
    fd_wrapper socket_fd{socket(AF_INET, SOCK_STREAM, 0)};
    if (socket_fd.fd == -1) {
        throw std::runtime_error(std::string{"Could not create socket: "} +
                                 strerror(errno));
    }

    server.sin_addr.s_addr = inet_addr(address.data());
    server.sin_family = AF_INET;
    server.sin_port = htons(static_cast<uint16_t>(stoi(port)));

    if (connect(socket_fd.fd, reinterpret_cast<sockaddr *>(&server),
                sizeof(server)) == -1) {
        throw std::runtime_error(std::string{"Could not connect to address: "} +
                                 strerror(errno));
    }
    client_state state;
    std::cout << "Connected successfully" << std::endl;
    std::cout << read_message(socket_fd.fd) << std::endl;
    std::string query;
    while (true) {
        if (state.sending == 0) {
            std::cout << "Enter command: ";
        }
        getline(std::cin, query);
        if (query == "EXIT" || !std::cin) {
            break;
        }
        if (state.sending > 0) {
            state.send_message += query;
            state.sending -= query.size();
            if (state.sending <= 0) {
                swap(query, state.send_message);
                state.send_message = "";
                state.sending = 0;
            } else {
                state.sending -= 1;
                state.send_message += '\n';
                continue;
            }
        }
        if (query.empty()) {
            continue;
        }
        write_message(socket_fd.fd, query);
        std::string server_reply2 = read_message(socket_fd.fd);
        std::vector<std::string> queries = parse_string(query);
        if (queries[0] == "SIZE" && server_reply2[0] == '+') {
            try {
                state.sending = stoll(queries.at(1));
            } catch (...) {
                std::cout << "Invalid argument";
            }
        }
        std::cout << server_reply2 << std::endl;
    }
}

static const std::string USAGE =
    R"SEQ(
This is a simple client with the support of SFTP.
Usage: client [address [port]]
Default address is 127.0.0.1
Default port is 115.
Other than usual SFTP commands you can use EXIT -- it will quit this client.
)SEQ";

int main(int argc, char *argv[])
{
    std::cout << USAGE << std::endl;
    std::string address = "127.0.0.1";
    std::string port = "115";
    if (argc >= 2) {
        address = argv[1];
    }
    if (argc >= 3) {
        port = argv[2];
    }
    try {
        run(address, port);
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
