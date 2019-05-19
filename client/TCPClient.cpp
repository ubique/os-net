//
// Created by ifkbhit on 07.05.19.
//

#include "TCPClient.h"
#include "../protocol/Protocol.h"

net::TCPClient::TCPClient(const std::string& ipv4, const std::string& port, const Protocol* aProtocol) : TCPSocket(ipv4,
                                                                                                              port),
                                                                                                    aProtocol(
                                                                                                            aProtocol) {}


void net::TCPClient::run(const data_t& data) {
    aProtocol->sender(getFileDescriptor(), data);
}

bool net::TCPClient::afterCreate(const sockaddr_in& in) {
    if (connect(getFileDescriptor(), (struct sockaddr*) &in, sizeof(in)) < 0) {
        return Utils::fail("Couldn't connect to " + fullAddress());
    }
    Utils::message("Connected to " + fullAddress());
    return true;
}
