#include "server.h"

server::server(const char* address, const char* port) : socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket_fd.get_fd() == -1) {
        throw std::runtime_error("Can't create socket");
    }

    memset(&socket_addr, 0, sizeof(sockaddr_in));

    try {
        socket_addr.sin_port = (uint16_t)(std::stoul(port));
    } catch(const std::exception &e) {
        throw std::runtime_error(e.what());
    }
    
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = inet_addr(address);

    if (bind(socket_fd.get_fd(), (sockaddr*)(&socket_addr), sizeof(sockaddr)) == -1) {
        throw std::runtime_error("Can't bind socket");
    }

    if (listen(socket_fd.get_fd(), 5) == -1) {
        throw std::runtime_error("Can't listen socket");
    }
}

void server::run() {
    while (true) {
        std::cout << "Сonnection..." << std::endl;
        
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(struct sockaddr_in);;
        fd_wrapper client_fd(accept(socket_fd.get_fd(), (sockaddr*)(&client_addr), &client_addr_size));
        if (client_fd.get_fd() == -1) {
            error("Can't accept client address");
            continue;
        }

        std::cout << "Client connected" << std::endl;

        while (true) {
            std::vector<char> buffer(BUF_SIZE);
            ssize_t read = recv(client_fd.get_fd(), buffer.data(), BUF_SIZE, 0);
            
            if (read > 0) {
                size_t attempt_number = 1;
                while (send(client_fd.get_fd(), buffer.data(), (size_t) (read), 0) != read) {
                    error("Can't send message");
                    ++attempt_number;
                    std::cerr << "Attempt number " << attempt_number << std::endl;
                }
            } else if (read == 0) {
                error("Client disconnected", false);
                break;
            } else if (read == -1) {
                error("Can't receive");
                continue;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2 && argv[1] != nullptr && argv[1] == "-help") {
        std::cerr << HELP << std::endl;
        return 0;
    }
    if (argc > 3) {
        error("Wrong usage", false, true, true);
    }

    try {
        std::string address = "127.0.0.1";
        std::string port = "10005";

        if (argc == 2) {
            address = argv[1];        
        }

        if (argc == 3) {
            port = argv[2];
        }

        server server(address.c_str(), port.c_str());
        server.run();
    } catch (const std::invalid_argument &e) {
        error(e.what(), true, true, true);
    } catch (const std::runtime_error& e) {
        error(e.what(), true, false, true);
    }
}
