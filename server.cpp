#include <iostream>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>

#define MAXCON 5

int main(int argc, char** argv)
{

    if (argc != 3 )
    {
        std::cout << "Wrong arguments: <host> <port> expected\n";
        return 0;
    }

    int sockd;
    if ((sockd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(atoi(argv[2])), .sin_addr = {.s_addr = inet_addr(argv[1])}};

    if (bind(sockd, (sockaddr*) &addr, sizeof(addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockd, MAXCON) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        static sockaddr_in client_addr{};
        static unsigned int client_len;
        static int clientSockd;
        static int msglen;
        char buffer[1024];
        bzero(buffer, 1024);
        std::cout << "Server is running..\n";
        if ((clientSockd = accept(sockd, (sockaddr*) &client_addr, &client_len)) == -1)
        {
            perror("accept");
            continue;
        }

        if ((msglen = recv(clientSockd, &buffer, sizeof(buffer), 0)) == -1)
        {
            perror("recv");
            continue;
        }

        if (strncmp(buffer, "exit", msglen) == 0)
        {
            if ((send(clientSockd, "Server has stopped.\n", 21, 0)) == -1)
            {
                perror("send");
            }
            close(clientSockd);
            std::cout << "Shutdown server.\n";
            return 0;
        }
        if ((send(clientSockd, &buffer, msglen, 0)) == -1)
        {
            perror("send");
        }
    }
}
