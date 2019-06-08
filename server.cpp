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

    char buff[bufferLen];
    struct sockaddr_in addr;
    int clientSockd;
    size_t msglen = 0;
    socklen_t addrlen;
    while (true) {
        bzero(buff, bufferLen);

        if ((clientSockd = accept(sockfd, (sockaddr *) &addr, &addrlen)) == -1) {
            perror("accept");
            continue;
        }

        auto buffSize = getSize(clientSockd);
        if (buffSize == nullptr) {
            perror("size recv");
            continue;
        }

        if(recvAll(clientSockd, buff, sizeof(buffSize))) {
            perror("msg recv");
            continue;
        }

        if (strncmp(buff, "exit", msglen) == 0) {
            if (sendMsg(clientSockd, EXIT, strlen(EXIT)))
                perror("exit send");
            cout << "End of work" << endl;
            return closeSocket(clientSockd);
        }

        if (sendMsg(clientSockd, buff, strlen(buff))) {
            perror("send");
            continue;
        }

        cout << buff << endl;
    }
}
