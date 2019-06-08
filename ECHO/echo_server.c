#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CONNECTIONS_QUEUE_LENGTH (100)
#define BUF_SIZE (1024)

#define FATAL_ERROR(msg) {perror(msg);exit(EXIT_FAILURE);}
#define SOFT_ERROR(msg) {perror(msg);return;}

/** Work with client. */
void server_client_serve(int fd) {
    char buf[BUF_SIZE];
    int received = 1;
    int sent;
    int tmp;

    while (0 < received) {
        if ((received = recv(fd, buf, BUF_SIZE, 0)) < 0) SOFT_ERROR("Failed to receive bytes from client.")
        sent = 0;
        while (sent < received) {
            if ((tmp = send(fd, buf + sent, received - sent, 0)) < 0) SOFT_ERROR("Failed to send bytes to client.")
            sent += tmp;
        }
    }
}

/** Loop for accepting client's connections. */
void server_loop(const int sfd) {
    while (1) {
        int cfd;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if ((cfd = accept(sfd, (struct sockaddr*) &client_addr, &client_addr_len)) < 0) FATAL_ERROR(
                "Failed to accept client connection.")
        fprintf(stdout, "Client connected:%s:%hu\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        server_client_serve(cfd);
        fprintf(stdout, "Client disconnected:%s:%hu\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        close(cfd);
    }
}

/** Return socket file descriptor of server for specified ip and port. Used IPv4 tcp protocol. */
int server_init(const char* ip, const char* port) {
    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) FATAL_ERROR("Failed to create socket.")

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    switch (inet_pton(AF_INET, ip, &server_addr.sin_addr)) {
        case -1: FATAL_ERROR("inet_pton")
        case 0:
            fprintf(stderr, "Wrong ip address: '%s'", ip);
            exit(EXIT_FAILURE);
        default: // SUCCESS
            break;
    }

    if (bind(sfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) FATAL_ERROR("Failed to bind socket.")
    printf("Server binded to:\n\tip:%s\n\tport:%hu\n\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    if (listen(sfd, MAX_CONNECTIONS_QUEUE_LENGTH) < 0) FATAL_ERROR("Failed to listen socket.")
    return sfd;
}

/** Enter point of program.
 * First argument must contain <IPv4> and
 * second argument must contain <PORT>. */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "USAGE: server <IPv4> <PORT>\n");
        exit(EXIT_FAILURE);
    }
    server_loop(server_init(argv[1], argv[2]));
    return EXIT_SUCCESS;
}
