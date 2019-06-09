//
// Created by Yaroslav on 04/06/2019.
//
//#include<bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <chrono>
#include "utils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void wrong_usage() {
    cout << "Usage: ./Task5_client [address] [message] ['code' or 'decode']";
}

void print_vec(const vector<char>& vec, int size) {
    for (int i = 0; i < size; i++) {
        cout << vec[i];
    }
    cout << "\n";
}

void check_error(int rc, const string &problem_name) {
    if (rc == -1) {
        cout << "error occurred with ";
        cout << problem_name << " in client\n";
        cout << strerror(errno) << "\n";
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        wrong_usage();
        return 0;
    }
    string name = argv[2];
//    cout << name;
    string key = argv[3];
    char size = name.length() + 1;
    char code;
    vector<char> message(size);

    if (key == "code") {
        code = 1;
    } else if (key == "decode") {
        code = 0;
    } else {
        wrong_usage();
        return 0;
    }
    for (int i = 0; i < size - 1; i++) {
        message[i] = static_cast<char>(name[i]);
//        message[i] = 'a' + i;

    }

    message[size - 1] = '\0';
    int s = socket(AF_INET, SOCK_STREAM, 0);
    check_error(s, "socket");
    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    check_error(inet_pton(AF_INET, argv[1], &server.sin_addr), "inet_pton");
    check_error(connect(s, (sockaddr *) (&server), sizeof(server)), "connect");
    print_vec(message, size - 1);
    size*=2;
    if (code) {
        size++;
    }
    fun_send(&size, 1, s, "client");
    fun_send(&message[0], size, s, "client");
    fun_recv(&message[0], size, s, "client");
    size /= 2;
    print_vec(message, size - 1);
    check_error(close(s), "close");
    return 0;
}