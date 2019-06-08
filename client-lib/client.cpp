//
// Created by Михаил Терентьев on 2019-05-13.
//


#include <string>
#include <vector>
#include "client.h"
#include "client_exception.h"
#include "../utils/data_builder.h"

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
    text += "\r\n";
    size_t process_sz = 0;
    while (process_sz < text.length()) {
        ssize_t current = send(d_socket.getDiscriptor(), text.data() + process_sz,
                               text.length() - process_sz, 0);
        if (current == -1) {
            throw client_exception("Can't send request");
        }
        process_sz += static_cast<size_t>(current);
    }
    data_builder builder;
    auto recp = builder.process(d_socket.getDiscriptor(), BF_SZ);
    if (recp.first == -1) {
        throw client_exception("Reading from fd error");
    }

    return recp.second;
}
