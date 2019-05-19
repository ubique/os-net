#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <algorithm>
#include <iostream>
#include <vector>

int main(int argc, char**argv) {
    if (argc < 4) {
        printf("Usage: os_client <server> <port> <message>\n");
        return 1;
    }

    int sock;

    {
        addrinfo hints;
        addrinfo* result;

        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags = 0;

        int ret = getaddrinfo(argv[1], NULL, &hints, &result);

        if (ret != 0) {
            fprintf(stderr, "Could not resolve host: %s\n", gai_strerror(ret));
        }

        addrinfo* rp;

        for (rp = result; rp != NULL; rp = rp->ai_next) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sock == -1) {
                fprintf(stderr, "Could create socket: %s\n", strerror(errno));
                continue;
            }

            ((sockaddr_in*)rp->ai_addr)->sin_port = htons(atoi(argv[2]));
            if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
                break;
            } else {
                fprintf(stderr, "Could not connect: %s\n", strerror(errno));
            }

            close(sock);
        }

        if (rp == NULL) {               /* No address succeeded */
            fprintf(stderr, "Connection on all addresses failed\n");
            exit(EXIT_FAILURE);
        }

        freeaddrinfo(result);           /* No longer needed */
    }

    int nwrtn;
    size_t offset = 0;
    std::string query = argv[3];
    query += '\n';

    while ((nwrtn = write(sock, query.c_str() + offset, query.length() - offset)) > 0 || errno == EINTR) {
        offset += nwrtn;
    }

    if (nwrtn < 0) {
        fprintf(stderr, "Could not write to socket: %s\n", strerror(errno));
        return 1;
    }

    printf("Sent: %s\n", query.c_str());

    char tmp[1024];
    std::string buffer;

    int nread;

    while ((nread = read(sock, tmp, sizeof(tmp))) > 0 || errno == EINTR) {
        std::copy(tmp, tmp + nread, std::inserter(buffer, buffer.end()));

        if (std::find(tmp, tmp + nread, '\n') != tmp + nread) {
            break;
        }
    }

    if (nread < 0) {
        fprintf(stderr, "Could not read from socket: %s\n", strerror(errno));
        return 1;
    }

    printf("Received: %s\n", buffer.c_str());

    return 0;
}
