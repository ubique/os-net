//
// Created by ifkbhit on 07.05.19.
//

#include <cstdio>
#include <sys/socket.h>
#include <iostream>
#include "ECHOProtocol.h"
#include <sstream>

bool net::ECHOProtocol::sender(const int& descriptor, const data_t& data) const {
    ssize_t sent;
    if ((sent = send(descriptor, data.data(), data.size(), 0)) != data.size()) {
        return Utils::fail("Couldn't send all data");
    }
    Utils::message("Sent " + std::to_string(sent) + " bytes");
    data_t echo;
    Utils::message("# Response from server");
    bool success = read(descriptor, echo, data.size());
    Utils::message((success ? "Success" : "Fail") + std::string(" read"));
    return success;
}

bool net::ECHOProtocol::receiver(const int& descriptor) const {
    data_t receivedData;

    Utils::message("Read request");
    bool success = read(descriptor, receivedData, -1);
    if (!success) {
        return false;
    }
    int sent;
    if ((sent = send(descriptor, receivedData.data(), receivedData.size(), 0)) != receivedData.size()) {
        return Utils::fail("Couldn't send all data back");
    }

    Utils::message("Sent " + std::to_string(sent) + " bytes");

    return true;
}

bool net::ECHOProtocol::read(const int& descriptor, data_t& receivedData, const int& targetSize) const {
    size_t bufferSize = 128;
    data_t buffer(bufferSize);
//    Utils::message("Start read");

    size_t bytesGet;
    long count = 0;
    while (targetSize == -1 || receivedData.size() < targetSize) {
//        Utils::message("Filling buffer...");
        bytesGet = recv(descriptor, buffer.data(), bufferSize, 0);
//        Utils::message("Done with " + std::to_string(bytesGet) + " bytes");
        if (bytesGet == -1) {
            return Utils::fail("Error while receiving (ECHOProtocol::receiver function)");
        } else {
            count += bytesGet;
            for (auto i = 0; i < bytesGet; i++) {
                receivedData.push_back(buffer[i]);
            }
        }
        if (bytesGet == 0 || bytesGet < bufferSize) {
            if (targetSize != -1 && receivedData.size() != targetSize) {
                return Utils::fail("Not all excepted data received");
            }
            break;
        }
    }

    std::stringstream received;
    received << "Received " << count << " bytes: \n";

    for (auto c: receivedData) {
        received << c;
    }
    Utils::message(received.str());
    return true;
}

