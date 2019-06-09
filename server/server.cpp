#include "server.h"

const int server::LISTEN_TIMEOUT = 10;
const size_t server::BUFFER_SIZE = 1024;

server::server(char* socket_name) : socket(), socket_binding(socket_name) {
    unlink(socket_binding.c_str());
    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_name, sizeof(address.sun_path) - 1);
    int res = bind(socket.get_fd(), reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr));
    if (res == -1) {
        throw_error("Unable to bind socket");
    }

    res = listen(socket.get_fd(), server::LISTEN_TIMEOUT);
    if (res == -1) {
        throw_error("Unable to listen socket");
    }
}

server::~server() {
    unlink(socket_binding.c_str());
}

void server::launch() {
    while (true) {
        raii_socket data_socket(accept(socket.get_fd(), nullptr, nullptr));
        char buffer[BUFFER_SIZE + 1];
        ssize_t ret = read(data_socket.get_fd(), buffer, BUFFER_SIZE);
        if (ret == -1) {
            print_error("Unable to recieve data");
        } else {
            buffer[BUFFER_SIZE] = 0;
            std::string message(buffer, buffer + ret);
            std::cout << "Server recieved " << message << '\n';
            size_t ptr = 0;
            while (ptr < message.size()) {
                ssize_t len = write(data_socket.get_fd(), buffer + ptr, message.size() - ptr);
                if (len == -1) {
                    print_error("Unable to send response");
                } else {
                    std::cout << "Server sent " << message.substr(ptr, len) << '\n';
                    ptr += len;
                }
            }
            if (!strncmp(buffer, "stop", 4)) {
                down = true;
            }
        }
        if (down) {
            std::cout << "Shutdown.\n";
            break;
        }
     }
}