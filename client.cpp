#include "utils.h"

using namespace std;

const size_t bufferLen = 1024;

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Expected port and message\n";
        return 1;
    }
    auto port = atoi(argv[1]);
    auto msg = argv[2];

    int sockfd = createSocket();

    setSocketOptions(sockfd);

    auto serv_addr = genStructFields(htonl(INADDR_LOOPBACK), port);
    if (connect(sockfd, (const struct sockaddr *) &serv_addr.second, serv_addr.first) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(msg);
    if (send(sockfd, msg, len, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    char buf[bufferLen];
    bzero(buf, bufferLen);
    if ((recv(sockfd, &buf, bufferLen, 0)) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    return closeSocket(sockfd);
}