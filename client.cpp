//
// Created by Georgy Rozovsky on 18/05/2019.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <string.h>

using namespace std;

const int BUFFER = 1024;
const int PORT = 8080;

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("an error occurred while creating the socket, terminating...");
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(303);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("an error occurred while connecting client socket, terminating...");
        return 404;
    }

    char info[] = "TestInfo";
    int sizeOfInfo[1] = { sizeof(info) };
    int sended = 0;
    while (sended != sizeof(sizeOfInfo)) {
        int send_b_count = send(sock, (char *) sizeOfInfo + sended, sizeof(sizeOfInfo) - sended, 0);
        if(send_b_count == -1) {
            cout << "an error occurred while sending message\n";
            break;
        }
        sended += send_b_count;
    }

    cout << "running..." << endl;
    char * info_pointer = info;
    int d_size = strlen(info);
    while(0 < d_size) {
        int sended = send(sock, info_pointer, d_size, 0);
        if(sended == -1) {
            cout << "an error occurred while sending message\n";
            break;
        }
        info_pointer += sended;
        d_size -= sended;
    }
    shutdown(sock, SHUT_WR);
    char buf[sizeof(info)];
    int received = 0;

    while(received < sizeof(info)) {
        int rec_b_count = recv(sock, buf+received, sizeof(info) - received, 0);
        if(rec_b_count == -1) {
            cout << "an error occurred while receiving message\n";
            break;
        }
        received += rec_b_count;
    }

    cout << buf << endl;
    close(sock);

    return 0;
}