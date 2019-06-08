#include "server.h"
#include "server_execption.h"
#include "../utils/data_builder.h"

server::server(char *address, uint16_t port) : d_socket(socket(AF_INET, SOCK_STREAM, 0)) {
    if (d_socket.isBroken()) {
        throw server_exception("Can't create socket ");
    }
    fillAddress(address, port);
    if (bind(
            d_socket.getDiscriptor(),
            reinterpret_cast<sockaddr *>(&saddress),
            sizeof(sockaddr))
        == -1) {
        throw server_exception("Can't bind socket");
    }

    if (listen(
            d_socket.getDiscriptor(),
            3) == -1) {
        throw server_exception("Can't listen socket");
    }
}

[[noreturn]] void server::run() {
    while (true) {
        std::cout << "Trying to connect....." << std::endl;
        sockaddr_in peer{};
        socklen_t peer_sz;
        wrapper fd(accept(d_socket.getDiscriptor(),
                          reinterpret_cast<sockaddr *>(&peer),
                          &peer_sz
        ));
        if (fd.isBroken()) {
            std::cerr << "Cannot accept peer";
            continue;
        }
        std::cout << inet_ntoa(peer.sin_addr) << " connected!" << std::endl;
        while (true) {
            data_builder acc;
            auto got = acc.process(fd.getDiscriptor(), BF_SZ);
            ssize_t received_sz = got.first;
            if (received_sz == 0) {
                std::cout << "Client disconnected\n";
                break;
            } else if (received_sz == -1) {
                std::cerr << "Can't receive information";
                continue;
            } else {
                got.second += "\r\n";
                size_t was_sent = 0;
                while (was_sent < static_cast<size_t>(received_sz)) {
                    ssize_t current =
                            send(fd.getDiscriptor(),
                                 got.second.data() + was_sent,
                                 static_cast<size_t>(received_sz) - was_sent,
                                 0);
                    if (current == -1) {
                        throw server_exception("Unable to send");
                    }
                    was_sent += static_cast<size_t>(current);
                }
            }
        }
    }
}

void server::fillAddress(char *address, uint16_t port) {
    memset(&saddress, 0, sizeof(sockaddr_in));
    saddress.sin_family = AF_INET;
    saddress.sin_addr.s_addr = inet_addr(address);
    saddress.sin_port = port;
}
