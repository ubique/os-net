#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "Wrong arguments: <host> <port> <message> expected" << std::endl;
        return 0;
    }

    int sockd;
    if ((sockd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(atoi(argv[2])), .sin_addr = {.s_addr = inet_addr(argv[1])}};
    if (connect(sockd, (sockaddr*) &addr, sizeof(addr)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    size_t sent = 0;
    while (sent != strlen(argv[3]))
    {
        int n = send(sockd, argv[3] +  sent, strlen(argv[3]) - sent, 0);
        if (n == -1)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        if (!n)
        {
            break;
        }
        sent += n;
    }

    char buffer[1024];

    size_t received = 0;
    std::cout << "answer from the server: ";
    while (received != strlen(argv[3]))
    {
        bzero(buffer, sizeof(buffer));
        int n = recv(sockd, &buffer, sizeof(buffer), 0);
        if (n == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        if (!n)
        {
            break;
        }
        received += n;
        for (size_t i = 0; i < n; ++i)
        {
            std::cout << buffer[i];
        }
    }
    std::cout << std::endl;
    if (close(sockd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
    return 0;
}
