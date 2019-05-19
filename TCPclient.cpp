//
// Created by domonion on 02.05.19.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <zconf.h>
#include <utils.hpp>
#include <vector>
#include <random>
#include <chrono>

using std::cout;
using std::endl;
using std::mt19937;
using std::vector;

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: ./server address\nExample: ./server 127.0.0.1";
        return 0;
    }
    mt19937 randomer(std::chrono::system_clock::now().time_since_epoch().count());
    int s = socket(AF_INET, SOCK_STREAM, 0);
    check_error(s, "socket");
    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    check_error(inet_pton(AF_INET, argv[1], &server.sin_addr), "inet_pton");
    check_error(connect(s, (sockaddr *) (&server), sizeof(server)), "connect");
    char message_size = randomer() % MAX_MESSAGE_LEN + 1;
    vector<char> message(message_size);
    for (char &i : message)
        i = 'A' + randomer() % 26;
    message.back() = '\0';
    cout << &message[0] << endl;
    doSend(&message_size, 1, s);
    doSend(&message[0], message_size, s);
    doRecv(&message[0], message_size, s);
    cout << &message[0] << endl;
    check_error(shutdown(s, SHUT_RDWR), "shutdown");
    check_error(close(s), "close");
    return 0;
}