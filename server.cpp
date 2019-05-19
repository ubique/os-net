//
// Created by Georgy Rozovsky on 18/05/2019.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

const int BUFFER = 1024;
const int PORT = 8080;

using namespace std;

int main() {

    int listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener < 0) {
        perror("an error occurred while creating the socket, terminating...");
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(303);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("an error occurred with bind, terminating...");
        return 2;
    }

    listen(listener, 1);

    while (true) {
        int accepted = accept(listener, nullptr, nullptr);
        if (accepted < 0) {
            perror("an error occurred while accepting, terminating...");
            return 3;
        }

        while (true) {
            char buff[BUFFER];

            int received = 0;
            int sizeof_info[1];
            while (received != sizeof(sizeof_info)) {
                int rec_b_count = recv(accepted, sizeof_info + received, sizeof(sizeof_info) - received, 0);
                if(rec_b_count == -1) {
                    cout << "an error occurred with receiving message size\n";
                    break;
                }
                received += rec_b_count;
            }

            received = 0;
            while(received < sizeof_info[0]) {
                int rec_b_count = recv(accepted, buff + received, sizeof_info[0] - received, 0);
                if(rec_b_count == -1) {
                    cout << "problem with receiving message\n";
                    break;
                }
                received += rec_b_count;
            }

            int sended = 0;

            while(sended < sizeof_info[0]) {
                int send_b_count = send(accepted, buff+sended, received - sended, 0);
                if(send_b_count == -1) {
                    cout << "an error occurred while sending message\n";
                    break;
                }
                sended += send_b_count;
            }

        }

        close(accepted);
    }

    return 0;
}