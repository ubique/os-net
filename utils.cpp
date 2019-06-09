//
// Created by Yaroslav on 08/06/2019.
//

#include "utils.h"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/socket.h>

using std::cout;
using std::string;



void fun_send(char *what, int amount, int where, string message) {
    int counter = 0;
    while (counter < amount) {
//        int sent = send(where, what + counter, amount - counter, MSG_NOSIGNAL);
        int sent = send(where, what + counter, amount - counter, SO_NOSIGPIPE);
        if (sent == -1) {
            cout << "error occurred with sent in " << message << "\n";
            cout << strerror(errno) << "\n";
            exit(0);
        }
        counter += sent;
    }
}

void fun_recv(char *where, int amount, int from, string message) {
    int counter = 0;
    while(counter < amount) {
//        int received = recv(from, where + counter, amount - counter, MSG_NOSIGNAL);
        int received = recv(from, where + counter, amount - counter, SO_NOSIGPIPE);
        if (received == -1) {
            cout << "error occurred with recv "<< message << "\n";
            cout << strerror(errno) << "\n";
            exit(0);
        }
        counter += received;
    }
}