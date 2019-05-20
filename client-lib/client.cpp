//
// Created by Михаил Терентьев on 2019-05-13.
//


#include <string>
#include <vector>
#include "client.h"
#include "client_exception.h"

using namespace std;

client::client(char *address, uint16_t port) : d_socket(-1) {
    d_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (d_socket.isBroken()) {
        throw client_exception("Can't create socket");
    }
    memset(&addr_socket, 0, sizeof(addr_socket));
    addr_socket.sin_port = port;
    addr_socket.sin_family = AF_INET;
    addr_socket.sin_addr.s_addr = inet_addr(address);
    socklen_t addr_size = sizeof(sockaddr_in);
    if (connect(d_socket.getDiscriptor(), reinterpret_cast<sockaddr *>(&addr_socket), addr_size) == -1) {
        throw client_exception("Can't connect to server");
    }
}


string client::request(string text) {
    auto text_length = static_cast<ssize_t>(text.length());
    if (send(d_socket.getDiscriptor(),
             text.c_str(),
             text.length(), 0)
        != text_length) {
        throw client_exception("Can't send request");
    }

    vector<char> response(BF_SZ);
    ssize_t was_read = recv(d_socket.getDiscriptor(), response.data(), BF_SZ, 0);
    if (was_read == -1) {
        throw client_exception("Unable to read from file descriptor");
    }
    response.resize(static_cast<unsigned long>(was_read));

    return string(response.data());
}
