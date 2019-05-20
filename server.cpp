#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>

using namespace std;


const int BUFFER_SIZE = 1024;

void check_error(int value, const char* message) {
    if (value == -1) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: ./server  host port" << endl;

    }


    int file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    check_error(file_descriptor, "socket");


    struct sockaddr_in server{};
    server.sin_family = AF_INET;


    int port;
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "Invalid port";
        exit(EXIT_FAILURE);
    }
    server.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server.sin_addr);



    check_error((bind(file_descriptor, (sockaddr *) (&server), sizeof(server))), "bind");
    check_error(listen(file_descriptor, SOMAXCONN), "listen");


    struct sockaddr_in client{};
    unsigned int client_length;
    int client_descriptor;

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    cout << "Server started" << endl;
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);

        client_descriptor = accept(file_descriptor, (sockaddr*) &client, &client_length);


        if (client_descriptor == -1) {
            perror("accept");
            continue;
        }

        int message_len = recv(client_descriptor, &buffer, sizeof(buffer), 0);

        cout << "received message: " << buffer << endl;

        if (message_len == -1) {
            perror("recv");
            continue;
        }

        if (send(client_descriptor, &buffer, message_len, 0) == -1) {
            perror("send");
            continue;
        }


        cout << "send message: " << buffer << endl;
    }
}