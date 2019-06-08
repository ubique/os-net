#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE (1024)

#define FATAL_ERROR(msg) {perror(msg);exit(EXIT_FAILURE);}

/** Send msg to server and recieve answer. */
void client_send_msg(const int fd, const char* msg) {
    unsigned int msg_len = strlen(msg);
    char buf[BUF_SIZE];
    int received = 0;
    int sent = 0;
    int tmp;

    while (sent < msg_len) {
        if ((tmp = send(fd, msg + sent, msg_len - sent, 0)) < 0) FATAL_ERROR("Failed to send bytes to server.")
        sent += tmp;
    }

    fprintf(stdout, "Received: ");
    while (received < msg_len) {
        if ((tmp = recv(fd, buf, BUF_SIZE - 1, 0)) < 1) FATAL_ERROR("Failed to receive bytes from server.")
        received += tmp;
        buf[tmp] = '\0';
        fprintf(stdout, "%s", buf);
    }
    fprintf(stdout, "\n");

    close(fd);
}

/** Return socket file descriptor for specified ip and port. Used IPv4 tcp protocol. */
int client_connect(const char* ip, const char* port) {
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

    if (connect(sfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) FATAL_ERROR(
            "Failed to connect with server.")

    return sfd;
}

/** Enter point of program.
 * First argument must contain <SERVER_IPv4>,
 * second argument must contain <SERVER_PORT> and
 * third argument must contain <TEXT> to send. */
int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "USAGE: client <SERVER_IPv4> <SERVER_PORT> <TEXT>\n");
        exit(EXIT_FAILURE);
    }
    client_send_msg(client_connect(argv[1], argv[2]), argv[3]);
    return EXIT_SUCCESS;
}
