#include <iostream>
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
        static int n;
        char buffer[1024];
        bzero(buffer, 1024);
        std::cout << "Server is running.." << std::endl;

        if ((clientSockd = accept(sockd, (sockaddr*) &client_addr, &client_len)) == -1)
        {
            perror("accept");
            continue;
        }

        n = recv(clientSockd, &buffer, sizeof(buffer), 0);

        while (n)
        {
            if (n == -1)
            {
                perror("recv");
                exit(EXIT_FAILURE);
            }
            size_t sent = 0;
            while (sent != n)
            {

                int num = send(clientSockd, buffer +  sent, n - sent, 0);
                if (num == -1)
                {
                    perror("send");

                    exit(EXIT_FAILURE);
                }
                if (!num)
                {
                    break;
                }
                sent += num;
            }
            if (!strcmp(buffer, "exit"))
            {
                if (close(clientSockd) == -1)
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                std::cout << "Shutdown server.\n";
                return 0;
            }
            bzero(buffer, 1024);
            n = recv(clientSockd, &buffer, sizeof(buffer), 0);
        }
    }
}
