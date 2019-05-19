//
// Created by ifkbhit on 07.05.19.
//

#include <sstream>
#include <unistd.h>
#include "TCPServer.h"

net::TCPServer::TCPServer(const std::string& ipv4, const std::string& port, const Protocol* aProtocol) : TCPSocket(ipv4,
                                                                                                              port),
                                                                                                    aProtocol(
                                                                                                            aProtocol) {}

void net::TCPServer::run(const data_t& data) {

    while (true) {
        Utils::message("Waiting for client");
        int clientDescriptor;
        struct sockaddr_in client{};
        socklen_t clientAddressLength = sizeof(client);
        if ((clientDescriptor = accept(getFileDescriptor(), (struct sockaddr*) &client,
                                       &clientAddressLength)) < 0) {
            Utils::fail("Couldn't accept connection from client");
            return;
        }
        std::stringstream ss;
        ss << "Client " << std::string(inet_ntoa(client.sin_addr)) << ":" << ntohs(client.sin_port); // uint16_t problem
        Utils::message(ss.str() + " connected");
        aProtocol->receiver(clientDescriptor);
        close(clientDescriptor);
        Utils::message(ss.str() + " disconnected");

    }
}

bool net::TCPServer::afterCreate(const sockaddr_in& in) {
    if (bind(getFileDescriptor(), (struct sockaddr*) &in, sizeof(in)) < 0) {
        return Utils::fail("Socket bind (bind function)");
    }
    Utils::message("Server bind to " + fullAddress());
    if (listen(getFileDescriptor(), CONNECTION_QUEUE_SIZE) < 0) {
        return Utils::fail("Listen socket (listen function)");
    }
    Utils::message("Server created");
    return true;
}

