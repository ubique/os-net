#include "utils.h"

using namespace std;

const size_t bufferLen = 1024;

const char *EXIT = "Exit";

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Expected port\n";
        return 1;
    }
    auto port = atoi(argv[1]);

    int sockfd = createSocket();

    setSocketOptions(sockfd);

    auto my_addr = genStructFields(INADDR_ANY, port);
    if (bind(sockfd, (struct sockaddr *) &my_addr.second, my_addr.first) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    cout << "listener on port " << port << endl;
    int backlog = SOMAXCONN;
    if (listen(sockfd, backlog) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char buf[bufferLen];
    struct sockaddr_in addr;
    int clientSockd;
    size_t msglen;
    socklen_t addrlen;
    while (true) {
        bzero(buf, bufferLen);
        if ((clientSockd = accept(sockfd, (sockaddr *) &addr, &addrlen)) == -1) {
            perror("accept");
            continue;
        }

        if ((msglen = recv(clientSockd, &buf, bufferLen, 0)) == -1) {
            perror("recv");
            continue;
        }

        if (strncmp(buf, "exit", msglen) == 0) {
            if (send(clientSockd, EXIT, strlen(EXIT), 0) == -1)
                perror("exit send");
            cout << "End of work" << endl;
            return closeSocket(clientSockd);
        }

        if ((send(clientSockd, &buf, msglen, 0)) == -1) {
            perror("send");
            continue;
        }
        cout << buf << endl;
    }
}