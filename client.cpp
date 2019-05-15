#include <iostream>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "Wrong arguments: <host> <port> <message> expected\n";
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

    if (send(sockd, argv[3], strlen(argv[3]), 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    bzero(buffer, 1024);

    if ((recv(sockd, &buffer, sizeof(buffer), 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    std::cout << "answer from the server: " << buffer << std::endl;
    close(sockd);
    return 0;
}
