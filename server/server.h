#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H
#include "../raii_socket.h"
#include <sys/un.h>

class server {
public:
    static const int LISTEN_TIMEOUT;
    static const size_t BUFFER_SIZE;
    server() = delete;
    server(char* socket_name);
    ~server();
    server(const server& other) = delete;
    server& operator=(const server& other) = delete;
    void launch();
private:
    raii_socket socket;
    struct sockaddr_un address;
    std::string socket_binding;
    bool down = false;
};


#endif //OS_NET_SERVER_H
