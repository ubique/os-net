#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <zconf.h>


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
        cout << "Usage: ./client host port" << endl;
        exit(EXIT_SUCCESS);
    }


    cout << "Enter a message:";
    string message;
    cin >> message;
    message += '\n';


    int file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    check_error(file_descriptor, "socket");


    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    check_error(connect(file_descriptor, (sockaddr *) (&server), sizeof(server)), "connect");




    int total_send = 0;
    while (total_send < message.size()) {
        int status = send(file_descriptor, message.data() + total_send, message.size() - total_send, 0);
        check_error(status, "send");
        total_send += status;
    }


    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    check_error(recv(file_descriptor, &buffer, sizeof(buffer), 0), "recv");

    cout << "answer from the server: " << buffer;
    close(file_descriptor);
    return 0;
}