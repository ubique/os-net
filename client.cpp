#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

const int CMAX = 2 * 1024, tries = 100;

short parse(const char *argv, const char *name) {
    short ans = 0;
    for (int i = 0; argv[i] != '\0'; i++) {
        if (argv[i] < '0' || argv[i] > '9') {
            fprintf(stderr, "%s must be a natural number\n", name);
            exit(0);
        }
        ans = 10 * ans + (argv[i] - '0');
    }
    return ans;
}

void my_close(int socket, const char *name) {
    int status = close(socket);
    for (int i = 0; i < tries; i++) {
        if (status != -1 || errno != EINTR) {
            break;
        }
        status = close(socket);
    }
    if (status == -1) {
        fprintf(stderr, "Can't close %s socket: %s\n", name, strerror(errno));
        exit(0);
    }
}

char buf[2 * CMAX], rbuf[2 * CMAX];


//first argument ip, second argument port
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong usage: expected two arguments!\n");
        return 0;
    }
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Can't create a socket: %s\n", strerror(errno));
        return 0;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(parse(argv[2], "Port"));
    int status = inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);
    if (status == 0) {
        fprintf(stderr, "%s isn't correct ipv4 adress\n", argv[1]);
        return 0;
    }
    if (status == -1) {
        fprintf(stderr, "Can't parse the ip: %s\n", strerror(errno));
        return 0;
    }
    status = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
    for (int i = 0; i < tries; i++) {
        if (status != -1 || errno != EINTR) {
            break;
        }
        status = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
    }
    if (status == -1) {
        if (errno == EADDRINUSE || errno == EISCONN || errno == EADDRNOTAVAIL) {
            fprintf(stderr, "Error while connecting: %s\n", strerror(errno));
        } else {
            fprintf(stderr, "Can't connect client socket: %s\n", strerror(errno));
        }
        return 0;
    }

    while (int cnt_read = read(0, buf, CMAX)) {  
        for (int i = 0; i < tries; i++) {
            if (cnt_read != -1 || errno != EINTR) {
                break;
            }
            cnt_read = read(client_socket, rbuf, CMAX);
        }
        if (cnt_read == 0) {
            break;
        }
        if (cnt_read == -1) {
            fprintf(stderr, "Can't read from stdin: %s\n", strerror(errno));
            break;
        }
        int sum = 0;
        while (sum < cnt_read) {
            int cnt_write = write(client_socket, buf + sum, cnt_read - sum);
            for (int i = 0; i < tries; i++) {
                if (cnt_write != -1 || errno != EINTR) {
                    break;
                }
                cnt_write = write(client_socket, buf + sum, cnt_read - sum);
            }
            if (cnt_write == -1) {
                fprintf(stderr, "Can't write into socket: %s\n", strerror(errno));
                break;
            }
            bool err = false;
            int ssum = 0;            
            while (ssum < cnt_read) {
                int cnt = read(client_socket, rbuf, CMAX);
                for (int i = 0; i < tries; i++) {
                    if (cnt != -1 || errno != EINTR) {
                        break;
                    }
                    cnt = read(client_socket, rbuf, CMAX);
                }
                ssum += cnt;
                if (cnt == 0) {
                    break;
                }
                if (cnt == -1) {
                    fprintf(stderr, "Can't read from client socket: %s\n", strerror(errno));
                    err = true;
                    break;
                }
                for (int i = 0; i < cnt; i++) {
                    printf("%c", rbuf[i]);
                }
            }
            sum += cnt_write;
            if (err) {
                break;
            }
        }
        printf("\n");
    }
    my_close(client_socket, "client");   
    return 0;
} 