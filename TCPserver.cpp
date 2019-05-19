//
// Created by domonion on 02.05.19.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <errno.h>
#include <zconf.h>
#include <cstring>
#include <string>
#include <vector>
#include <utils.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: ./server address\nExample: ./server 127.0.0.1";
        return 0;
    }
    int master = socket(AF_INET, SOCK_STREAM, 0);
    check_error(master, "socket");
    struct sockaddr_in server{}, client{};
    socklen_t size = sizeof(sockaddr_in);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    check_error(inet_pton(AF_INET, argv[1], &server.sin_addr), "inet_pton");
    check_error(bind(master, (sockaddr *) (&server), size), "bind");
    check_error(listen(master, SOMAXCONN), "listen");
    while (true) {
        int slave = accept(master, (sockaddr *) (&client), &size);
        check_error(slave, "accept");
        char size;
        doRecv(&size, 1, slave);
        vector<char> data(size);
        doRecv(&data[0], size, slave);
        cout << &data[0] << endl;
        for (char &i : data) {
            i += abs('A' - 'a');
        }
        data.back() = '\0';
        doSend(&data[0], size, slave);
        check_error(shutdown(slave, SHUT_RDWR), "shutdown");
        check_error(close(slave), "close");
    }
}