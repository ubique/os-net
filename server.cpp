//
// Created by anastasia on 19.05.19.
//

#include <cstring>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct server_exception : std::runtime_error {
    explicit server_exception(const std::string &cause)
            : std::runtime_error(cause + ": " + strerror(errno)){}

};

class vector;

class Server {
public:
    Server() = default;

    Server(char* address, uint16_t port) {
        memset(&server_addr, 0, sizeof(sockaddr_in));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(address);
        server_addr.sin_port = port;

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw server_exception("Cannot create socket");
        }


        if (bind(socket_fd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(sockaddr)) == -1) {
            throw server_exception("Cannot bind server");
        }

        if (listen(socket_fd, 3) == -1) {
            throw server_exception("Cannot listen socket");
        }
    }


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    void run() {
        while(true) {
            std::cout << "Waiting for connection.." << std::endl;
            sockaddr_in addr{};
            socklen_t  addr_size;
            int connection(accept(socket_fd, reinterpret_cast<sockaddr *>(&addr), &addr_size));
            if (connection == -1) {
                std::cerr << "Cannot accept connection";
                continue;
            }
            std::cout << "Connected" << std::endl;

            while(true) {
                std::vector<char> ans(static_cast<unsigned long>(BUFFER_SIZE));
                ssize_t sz = recv(connection, ans.data(), (size_t) BUFFER_SIZE, 0);
                if (sz == -1) {
                    std::cerr << "Cannot read" << std::endl;
                    continue;
                } else if (sz == 0) {
                    std::cout << "Client disconnected" << std::endl;
                    break;
                } else {
                    while (send(connection, ans.data(), static_cast<size_t>(sz), 0) != sz) {
                        std::cerr << "Cannot send the answer\n" << "Retrying" << std::endl;
                    }
                }
            }
        }
    }

#pragma clang diagnostic pop

    ~Server() {
        close(socket_fd);
    }


private:
    struct sockaddr_in server_addr{};
    int socket_fd{};


    const int BUFFER_SIZE = 2048;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Two arguments expected: address and port.";
        exit(EXIT_FAILURE);
    }
    try {
        Server server(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        server.run();
    } catch (server_exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
