#include "client.hpp"

tftp_client::tftp_client(std::string const &address, size_t server_port) : socket_desc(), server_address{0} {
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (connect(&socket_desc.get_descriptor(), reinterpret_cast<sockaddr*>(&server_address), sizeof(sockaddr_in)) == -1) {
        // throw something
    }
}

int main() {

}
