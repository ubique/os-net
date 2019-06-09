#include "client.h"
#include "raii_socket.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

const size_t client::BUFFER_SIZE = 1024;

client::client(char* socket_name) {
    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_name, sizeof(address.sun_path) - 1);
}

std::string client::send_and_receive(std::string const& message) {
    char buffer[BUFFER_SIZE + 1];
    size_t ptr = 0;
    std::string result;

    while (ptr < message.size()) {
        raii_socket data_socket;
        ssize_t ret = connect(data_socket.get_fd(), reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr));
        if (ret == -1) {
            throw_error("Unable to connect to server");
        }

        size_t len = std::min(BUFFER_SIZE, message.size() - ptr);
        ret = write(data_socket.get_fd(), message.c_str() + ptr, len);
        if (ret == -1) {
            throw_error("Unable to send data");
        }
        std::string request = message.substr(ptr, ret);
        std::cout << "Client sent " << request << std::endl;
        ptr += ret;

        int ptr2 = 0;
        while (ptr2 < request.size()) {
            len = std::min(BUFFER_SIZE, message.size() - ptr2);

            ret = read(data_socket.get_fd(), buffer, len);
            buffer[BUFFER_SIZE] = 0;
            if (ret == -1) {
                throw_error("Unable to receive data");
            }
            std::string response(buffer, buffer + ret);
            std::cout << "Client received " << response << "\n" <<  std::endl;

            ptr2 += ret;
            result += response;
        }
    }
    return result;
}