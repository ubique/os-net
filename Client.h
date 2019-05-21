#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <cstring>
#include <cstdint>

struct ClientException: std::runtime_error {
    ClientException(std::string const& msg) :
            std::runtime_error(msg+ ", " + strerror(errno)) {}
};

struct Client {
    explicit Client(uint16_t port);
    ~Client();

    void run();

    void sendRequest(std::string const& data);
    std::string getResponse();

private:
    const uint16_t port;
    const int sfd;

    static const size_t BUFFER_SIZE = 4098;

    char buffer[BUFFER_SIZE];
};


#endif //OS_NET_CLIENT_H
