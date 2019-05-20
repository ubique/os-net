
#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H


#include <cstdint>

struct Client {
    explicit Client(uint16_t port);
    ~Client();

    void run();

    void sendRequest(std::string const& data);
    std::string getResponse();

private:
    const uint16_t port;
    const int cfd;

    static const size_t BUFFER_SIZE = 4098;

    char buffer[BUFFER_SIZE];
};


#endif //OS_NET_CLIENT_H
