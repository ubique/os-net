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
            std::vector<uint8_t> for_read_len(1);
            ssize_t read_message_length = recv(client_fd.get_fd(), for_read_len.data(), 1, 0);
            
            if (read_message_length > 0) {
                size_t message_length = for_read_len[0];

                std::string message(message_length, ' ');
                size_t message_length_read = 0;
                bool read_failed = false;

                while (message_length_read < message_length) {
                    size_t left_read = message_length - message_length_read;
                    ssize_t cur_read = recv(client_fd.get_fd(), (void *) (message.data() + message_length_read), left_read, 0);
                    
                    if (cur_read == -1) {
                        error("Can't receive message");
                        read_failed = true;
                        break;
                    } else if (cur_read == 0) {
                        error("Client disconnected", false);
                        read_failed = true;
                        break;
                    }

                    message_length_read += (size_t) cur_read;
                }

                if (read_failed) {
                    break;
                }

                size_t message_length_sent = 0;
                size_t attempt_number = 1;
                bool send_failed = false;

                while (message_length_sent < message_length) {
                    size_t left_send = message_length - message_length_sent;
                    ssize_t cur_sent = send(client_fd.get_fd(), message.c_str() + message_length_sent, left_send, 0);

                    if (cur_sent < 0) {
                        error("Can't send message part");
                        send_failed = true;
                        break;
                    } else if (cur_sent == 0) {
                        error("Client disconnected", false);
                        send_failed = true;
                        break; 
                    }
                    
                    message_length_sent += (size_t) cur_sent;
                }

                if (send_failed) {
                    break;
                }
            } else if (read_message_length == 0) {
                error("Client disconnected", false);
                break;
            } else if (read_message_length == -1) {
                error("Can't receive message length");
                break;
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
