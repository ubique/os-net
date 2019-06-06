#include "client.h"

client::client(const char* addr, const char* port) : socket_client_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket_client_fd.get_fd() == -1) {
        throw std::runtime_error("Can't create socket");
    }
    
    memset(&socket_addr, 0, sizeof(sockaddr_in));
    
    try {
        socket_addr.sin_port = std::stoul(port);
    } catch(const std::exception &e) {
        throw std::runtime_error(e.what());
    }

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = inet_addr(addr);

    if (connect(socket_client_fd.get_fd(), (sockaddr*)(&socket_addr), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Can't connect to server");
    }
}

void client::run() {
    while (!std::cin.eof()) {
        std::cout << "Request:  ";
        std::cout.flush();

        std::string message;
        std::getline(std::cin, message);

        if (message == "-exit") {
            break;
        }

        if (message == "\n" || message == "") {
            continue;
        }

        size_t message_length = (size_t) (message.length());
        
        if (message_length > MAX_LEN_MESSAGE) {
            error("Too large message", false);
            continue;
        }

        std::vector<uint8_t> for_send_len = {(uint8_t) message_length};
        if (send(socket_client_fd.get_fd(), for_send_len.data(), 1, 0) != 1) {
            throw std::runtime_error("Can't send message");
        }

        size_t message_length_sent = 0;
        while (message_length_sent < message_length) {
            size_t left_send = message_length - message_length_sent;
            ssize_t cur_sent = send(socket_client_fd.get_fd(), message.c_str() + message_length_sent, left_send, 0);

            if (cur_sent < 0) {
                throw std::runtime_error("Can't send message");
            }

            message_length_sent += (size_t) cur_sent;
        }

        std::string response(message_length, ' ');
        size_t message_length_read = 0;

        while (message_length_read < message_length) {
            size_t left_read = message_length - message_length_read;
            ssize_t cur_read = recv(socket_client_fd.get_fd(), (void *) (response.data() + message_length_read), left_read, 0);
            
            if (cur_read == -1) {
                throw std::runtime_error("Can't read fd");
            } else if (cur_read == 0) {
                break;
            }

            message_length_read += (size_t) cur_read;
        }
        //response.resize(read_);

        std::cout << "Response: " << response.data() << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 2 && argv[1] != nullptr && argv[1] == "-help") {
        std::cerr << HELP << std::endl;
        return 0;
    } else if (argc == 2 && argv[1] != nullptr && argv[1] == "-exit") {
        return 0;
    }

    if (argc > 3) {
        error("Wrong usage", false, true, true);
    }

    try {
        std::string adress = "127.0.0.1";
        std::string port = "10005";

        if (argc == 2) {
            adress = argv[1];        
        }

        if (argc == 3) {
            port = argv[2];
        }

        client client(adress.c_str(), port.c_str());
        std::cout << "Echo-client: type message" << std::endl;
        
        client.run();
    } catch (std::runtime_error& e) {
        error(e.what(), true, false, true);
    }

    return 0;
}
