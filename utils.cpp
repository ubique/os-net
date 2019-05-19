//
// Created by domonion on 02.05.19.
//

#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/socket.h>

using std::cout;
using std::endl;
using std::string;

void check_error(int rc, const string &additional) {
    if (rc == -1) {
        int error = errno;
        cout << additional << endl;
        cout << strerror(error) << endl;
        exit(0);
    }
}

void doSend(char *what, int amount, int where) {
    int counter = 0;
    while (counter < amount) {
        int sent = send(where, what + counter, amount - counter, MSG_NOSIGNAL);
        check_error(sent, "send");
        counter += sent;
    }
}

void doRecv(char *where, int amount, int from) {
    int counter = 0;
    while(counter < amount) {
        int received = recv(from, where + counter, amount - counter, MSG_NOSIGNAL);
        check_error(received, "recv");
        counter += received;
    }
}