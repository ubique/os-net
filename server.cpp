#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "utils.h"

using std::cout;
using std::stoi;
using std::cerr;
using std::string;
using std::vector;

void get_args(char *argv[], string &host, int &port) {
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "Invalid port";
        exit(EXIT_FAILURE);
    }
    host = argv[1];
}

void transform_msg(string &msg) {
    if (msg.empty())
        return;
    msg = "Z" + msg.substr(1, msg.size() - 1);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./os-net-server [host] [port]";
        exit(EXIT_FAILURE);
    }

    int port;
    string host;
    get_args(argv, host, port);

    //creating socket file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    utils::check(fd, "in socket");

    //initializing server
    struct sockaddr_in server{}, client{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    socklen_t s_len = sizeof(sockaddr_in);
    char buffer_size;
    string buffer;

    utils::check(inet_pton(AF_INET, host.data(), &server.sin_addr), "in inet_pton");
    utils::check(bind(fd, (sockaddr *) (&server), s_len), "in bind");
    utils::check(listen(fd, SOMAXCONN), "in listen");

    //running...
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    cout << "Running on port " << port << std::endl;
    while (true) {
        int cd = accept(fd, (sockaddr *) (&client), &s_len);
        utils::check(cd, "in accept");

        utils::receive_msg(&buffer_size, 1, cd);
        buffer.clear();
        buffer.resize(buffer_size, 0);
        utils::receive_msg(&buffer[0], buffer_size, cd);
        utils::print_msg(buffer);

        transform_msg(buffer);
        utils::send_msg(&buffer[0], buffer_size, cd);

        utils::check(shutdown(cd, SHUT_RDWR), "in client descriptor shutdown");
        utils::check(close(cd), "in client descriptor close");
    }
#pragma clang diagnostic pop
}