#include <iostream>
#include <arpa/inet.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>



using namespace std;

int const BUFFER_SIZE = 8192;

int main(int argc, char** argv){
    if (argc != 3){
        cerr << "Usage: ./client host port\n";
        exit(EXIT_FAILURE);
    }


    int fileDes = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDes == -1){
        cerr << "Can't create socket\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));


    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != -1){
        cerr << "Invalid host\n";
        exit(EXIT_FAILURE);
    }


    if (connect(fileDes, (sockaddr *) (&server), sizeof(server)) == -1){
        cerr << "Can't connect\n";
        return 0;
    }

    cout << "Enter the message: \n";
    string message;
    cin >> message;

    message += char(0);

    int sent = 0;
    int size = message.size();

    while(sent < size) {
        int cur = send(fileDes, message.data() + sent, message.size() - sent, 0);
        if (cur == -1){
            cerr << "Error while sending " + message + "\n";
            exit(EXIT_FAILURE);
        }
        sent += cur;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    char last = 'a';
    int got = 0;
    while (last != 0) {
        int cur = recv(fileDes, &buffer + got, sizeof(buffer) - got, 0);
        if (cur == -1){
            cerr << "Errer occured while receiving\n";
            exit(EXIT_FAILURE);
        }
        got += cur;
        last = buffer[got - 1];
    }

    std::cout << "You sent: " << buffer << std::endl;
    close(fileDes);
    return 0;
}
