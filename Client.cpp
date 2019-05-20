//
// Created by anastasia on 19.05.19.
//

#include <cstring>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct client_exception : std::runtime_error {
    explicit client_exception(const std::string &cause)
            : std::runtime_error(cause + ": " + strerror(errno)){}

};

class Client {
public:
    Client() = default;

    Client(char* address, uint16_t port) {
        memset(&server_addr, 0, sizeof(sockaddr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(address);
        server_addr.sin_port = port;

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw client_exception("Cannot create socket");
        }

        if (connect(socket_fd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(sockaddr_in)) == -1) {
            throw client_exception("Cannot connect to server");
        }
    }

    std::string request(const std::string &text) {
        if (send(socket_fd, text.c_str(),  text.length(), 0) == -1) {
            throw client_exception("Cannot send request");
        }

        std::vector<char> ans((unsigned long) BUFFER_SIZE);
        ssize_t sz = recv(socket_fd, ans.data(), (size_t) BUFFER_SIZE, 0);
        if (sz == -1) {
            throw client_exception("Cannot read from socket");
        }
        ans.resize(static_cast<unsigned long>(sz));

        return std::string(ans.data());
    }

    ~Client() {
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
        Client client(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        std::string text;
        while (std::getline(std::cin, text)) {
            if (text == "exit") {
                break;
            }
            std::cout << "Echo: " << client.request(text) << std::endl;
        }
    } catch (client_exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}