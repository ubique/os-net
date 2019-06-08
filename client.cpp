#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "utils.h"

using std::cin;
using std::cout;
using std::stoi;
using std::cerr;
using std::string;
using std::vector;

void get_args(char *argv[], string &host, int &port) {
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "Invalid port\n";
        exit(EXIT_FAILURE);
    }
    host = argv[1];
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./os-net-client [host] [port]";
        exit(EXIT_FAILURE);
    }

    int port;
    string host;
    get_args(argv, host, port);

    //creating socket file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    utils::check(fd, "in socket");

    struct sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    socklen_t s_len = sizeof(sockaddr_in);

    utils::check(inet_pton(AF_INET, host.data(), &server.sin_addr), "in inet_pton");
    utils::check(connect(fd, (sockaddr *) (&server), sizeof(server)), "in connect");

    cout << "Enter message:\n";
    string msg;
    cin >> msg;
    if (msg.size() >= 127) {
        cerr << "Size of the message must be less than 127\n";
        exit(EXIT_FAILURE);
    }
    char msg_size = msg.size();

    utils::send_msg(&msg_size, 1, fd);
    utils::send_msg(&msg[0], msg_size, fd);

    utils::receive_msg(&msg[0], msg_size, fd);
    utils::print_msg(msg);

    utils::check(shutdown(fd, SHUT_RDWR), "in descriptor shutdown");
    utils::check(close(fd), "in descriptor close");
    return 0;
}