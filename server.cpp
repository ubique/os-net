#include "server.h"
#include "client_manager.h"

#include <iostream>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <cstring>

socks5_server::socks5_server(uint16_t port,
                             sig_atomic_t volatile& termination_marker)
    : port(port),
      termination_marker(termination_marker)
{}

static int set_nonblocking(int sfd)
{
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if(flags == -1)
    {
        std::cerr << "UwU Error getting client socket flags: "
                  << strerror(errno) << std::endl;
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if(s == -1)
    {
        std::cerr << "Error setting client socket flags to nonblocking state OwO: "
                  << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

void socks5_server::run() noexcept
{
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        std::cerr << "Could not create socket " << strerror(errno) << std::endl;
        return;
    }

    std::cerr << "Server socket created" << std::endl;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if(bind(socket_desc,
            reinterpret_cast<struct sockaddr *>(&server),
            sizeof(server)) < 0)
    {
        std::cerr << "Could not bind server socket " << strerror(errno) << std::endl;
        return;
    }

    std::cerr << "Server socket bind done." << std::endl;

    listen(socket_desc, 1000);

    client_manager mgr(termination_marker);
    c = sizeof(struct sockaddr_in);
    while(!termination_marker
          && (client_sock = accept(socket_desc,
                                   reinterpret_cast<struct sockaddr *>(&client),
                                   reinterpret_cast<socklen_t *>(&c))) != -1)
    {
        if (set_nonblocking(client_sock) == 0)
            mgr.add_client(client_sock);
        else
            close(client_sock);
    }

    if (!termination_marker)
        std::cerr << "Could not connect client: " << strerror(errno) << std::endl;
}


