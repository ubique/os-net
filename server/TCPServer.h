#pragma once

#include "../tcp/TCPSocket.h"
#include "../protocol/Protocol.h"

namespace net {
    class TCPServer : public net::tcp::TCPSocket {

    public:
        TCPServer(const std::string& ipv4, const std::string& port, const Protocol* aProtocol);

    protected:
        bool afterCreate(const sockaddr_in& in) override;

    public:
        void run(const data_t& data) override;

    private:

        int CONNECTION_QUEUE_SIZE = 20;
        const Protocol* aProtocol;

    };

}