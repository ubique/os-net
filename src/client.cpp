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
    size_t receiving = 0;
    bool accepting = false;
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
    char server_reply[BUFFER_SIZE];

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    fd_wrapper wrapper{socket_fd};
    if (socket_fd == -1) {
        throw std::runtime_error(std::string{"Could not create socket: "} +
                                 strerror(errno));
    }

    server.sin_addr.s_addr = inet_addr(address.data());
    server.sin_family = AF_INET;
    server.sin_port = htons(static_cast<uint16_t>(stoi(port)));

    if (connect(socket_fd, reinterpret_cast<sockaddr *>(&server),
                sizeof(server)) == -1) {
        throw std::runtime_error(std::string{"Could not connect to address: "} +
                                 strerror(errno));
    }
    client_state state;
    std::cout << "Connected successfully" << std::endl;
    if (recv(socket_fd, server_reply, 4096, 0) == -1) {
        throw std::runtime_error(
            std::string{"Failed to retrieve information about server: "} +
            strerror(errno));
    }
    std::cout << server_reply << std::endl;
    std::string query;
    while (true) {
        while (state.accepting && state.receiving > 0) {
            memset(server_reply, 0, BUFFER_SIZE);
            ssize_t result = recv(socket_fd, server_reply, BUFFER_SIZE, 0);
            if (result < 0) {
                throw std::runtime_error(
                    std::string{"Failure while receiving. Disconnect. "} +
                    strerror(errno));
            }
            std::cout << std::string{server_reply, static_cast<size_t>(result)}
                      << std::flush;
            state.receiving -=
                std::min(state.receiving, static_cast<size_t>(result));
        }
        if (state.accepting) {
            std::cout << std::endl;
            state.accepting = false;
        }
        std::cout << "Enter command: ";
        getline(std::cin, query);
        if (query == "EXIT" || !std::cin) {
            break;
        }
        if (send(socket_fd, query.data(), query.size(), 0) == -1) {
            throw std::runtime_error(
                std::string{"Failure while sending. Disconnect. "} +
                strerror(errno));
        }
        std::vector<std::string> queries = parse_string(query);
        memset(server_reply, 0, sizeof(server_reply));
        ssize_t received = recv(socket_fd, server_reply, BUFFER_SIZE, 0);
        if (received < 0) {
            throw std::runtime_error(
                std::string{"Failure while receiving. Disconnect. "} +
                strerror(errno));
        }
        if (queries[0] == "SEND") {
            state.receiving -=
                std::min(static_cast<size_t>(received), state.receiving);
            if (strncmp(server_reply, "-Error occurred while reading a file",
                        BUFFER_SIZE) == 0) {
                state.receiving = 0;
            } else {
                state.accepting = true;
                std::cout << std::string{server_reply, BUFFER_SIZE}
                          << std::flush;
                continue;
            }
        }
        std::cout << std::string{server_reply, BUFFER_SIZE} << std::endl;
        if (queries[0] == "RETR") {
            try {
                state.receiving = stoull(std::string{server_reply});
            } catch (...) {
                std::cout << "Invalid argument" << std::endl;
            }
        }
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
