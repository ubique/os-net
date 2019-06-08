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
    name += '\n';
    if (name.size() > 70) {
        cout << "Too long name\n";
        exit(EXIT_FAILURE);
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

    int send_cnt = 0;
    int cnt = 1;
    while (send_cnt != name.size() && cnt > 0) {
        cnt = send(sockfd, name.data() + send_cnt, name.size() - send_cnt, 0);
        send_cnt += cnt;
    }
    if (cnt < 0)
        print_error_and_exit("Sending failed", sockfd);

    char buf[101];
    int recv_cnt = 0;
    while ((cnt = recv(sockfd, buf + recv_cnt, 100 - recv_cnt, 0)) > 0) {
        recv_cnt += cnt;
        if (buf[recv_cnt] == '\n')
            break;
    }
    if (cnt < 0)
        print_error_and_exit("Receiving failed", sockfd);
    buf[recv_cnt - 1] = '\0';
    cout << buf << '\n';

    close_socket(sockfd);
    return 0;
}
