#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void close_socket(int fd) {
    if (close(fd) < 0)
        perror("Cannot close socket");
}

void print_error_and_exit(string message, int fd) {
    perror(message.c_str());
    close_socket(fd);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        cout << "Usage: client <addr> <port> <name>\n";
        exit(EXIT_FAILURE);
    }
    string name = argv[3];
    for (int i = 4; i < argc; i++) {
        name += ' ';
        name += argv[i];
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (!inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)) {
        cout << "Bad address " << argv[1] << '\n';
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create file descriptor for socket");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr*)  &serv_addr, sizeof serv_addr) < 0)
        print_error_and_exit("Cannot connect to socket", sockfd);
    if (send(sockfd, name.data(), name.size(), 0) < 0)
        print_error_and_exit("Sending failed", sockfd);

    char buf[101];
    int sz = recv(sockfd, buf, 100, 0);
    if (sz < 0)
        print_error_and_exit("Receiving failed", sockfd);
    buf[sz] = '\0';
    cout << buf << '\n';

    close_socket(sockfd);
    return 0;
}
