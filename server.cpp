#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>

using namespace std;


const int BUFFER_SIZE = 8192;


int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: ./server  host port" << endl;
        exit(EXIT_FAILURE);
    }


    int fileDes = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDes == -1){
        cerr << "Can't create socket\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(stoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != -1){
        cerr << "Invalid host\n";
        exit(EXIT_FAILURE);
    }


    if (bind(fileDes, (sockaddr *) (&server), sizeof(server)) == -1) {
        cerr << "Error occured while bind\n";
        exit(EXIT_FAILURE);
    }

    if (listen(fileDes, SOMAXCONN) == -1){
        cerr << "Error occured while listen\n";
        exit(EXIT_FAILURE);
    }


    struct sockaddr_in client{};
    unsigned int client_length;
    int clientDes;

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    cout << "Server started" << endl;
    bool working = true;
    while (working) {
        memset(buffer, 0, BUFFER_SIZE);

        clientDes = accept(fileDes, (sockaddr*) &client, &client_length);

        if (clientDes == -1) {
            cerr << "Can't accept\b";
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);

        char last = 'a';
        int got = 0;
        while (last != 0) {
            int cur = recv(fileDes, &buffer + got, sizeof(buffer), 0);
            if (cur == -1){
                cerr << "Errer occured while receiving\n";
                exit(EXIT_FAILURE);
            }
            got += cur;
            last = buffer[got - 1];
        }

        int sent = 0;

        int size = got;

        while(sent < size) {
            int cur = send(fileDes, buffer + sent, got - sent, 0);
            if (cur == -1){
                cerr << "Error while sending\n";
                exit(EXIT_FAILURE);
            }
            sent += cur;
        }


        cout << "recieve and send message: " << buffer << endl;
    }
}
