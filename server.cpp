#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define BACKLOG 100
#define BUF_SIZE 1000

using std::string;
using std::cout;
using std::endl;

void resolve(const string& addr, const string& port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Cannot create socket");
        return;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    if (inet_aton(addr.data(), &server.sin_addr) == 0) {
        cout << "Server's address is invalid" << endl;
        return;
    }
    try {
        server.sin_port = std::stoul(port);
    } catch (std::invalid_argument& e) {
        cout << "Server's port is invalid" << endl;
        return;
    }

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(sockaddr_in)) == -1) {
        perror("Cannot bind to socket");
        return;
    }
    if (listen(sockfd, 10) == -1) {
        perror("Cannot listen socket");
        return;
    }

    while (true) {
        cout << "Waiting for connection ..." << endl;
        struct sockaddr_in user;
        socklen_t user_size;
        int resultfd = accept(sockfd, reinterpret_cast<sockaddr*>(&user), &user_size);
        if (resultfd == -1) {
            perror("Cannot accept connection");
            continue;
        }
        cout << inet_ntoa(user.sin_addr) << " connected" << endl;
        while (true) {
            cout << "--------------------------------------" << endl;
            cout << "Waiting for message ..." << endl;
            char buffer[BUF_SIZE];
            memset(buffer, 0, sizeof(buffer));
            ssize_t read = recv(resultfd, buffer, BUF_SIZE, 0);
            if (read == -1) {
                perror("Error during receiving message");
                continue;
            } else if (read == 0) {
                cout << "User disconnected" << endl;
                break;
            } else {
                cout << "Received: " << string(buffer) << endl;
                while (send(resultfd, buffer, read, 0) != read) {
                    perror("Cannot send echo message, trying again");
                }
                cout << "Echo message is sent" << endl;
            }
        }
        while (close(resultfd) == -1) {
            perror("Cannot close connection socket, trying again");
        }
        cout << "++++++++++++++++++++++++++++++++++++++" << endl;
    }
    while (close(sockfd) == -1) {
        perror("Cannot close server socket, trying again");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " + string(argv[0]) + " [addr] [port]" << endl;
        return 0;
    }
    resolve(argv[1], argv[2]);
}
