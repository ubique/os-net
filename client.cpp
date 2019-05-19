#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cstring>

#define BUF_SIZE 1000

using std::string;
using std::cin;
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

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(sockaddr_in)) == -1) {
        perror("Cannot connect to server");
        return;
    }
    cout << "Connected to server" << endl;

    cout << "Write request message and wait for response (to exit - just press ENTER)" << endl;
    while (true) {
        cout << "Request: ";
        string request;
        getline(cin, request);
        if (request.empty()) {
            break;
        }
        if (send(sockfd, request.c_str(), request.length(), 0) != request.length()) {
            perror("Cannot send message");
            break;
        }
        char buffer[BUF_SIZE];
        memset(buffer, 0, sizeof(buffer));
        ssize_t read = recv(sockfd, buffer, BUF_SIZE, 0);
        if (read == -1) {
            perror("Error during receiving message");
            continue;
        } else if (read == 0) {
            cout << "Cannot receive message, server is down" << endl;
            break;
        }
        cout << "Response: " << string(buffer) << endl;
    }
    while (close(sockfd) == -1) {
        perror("Cannot close socket, trying again");
    }
    cout << "Disconnected from server" << endl;
}

int main() {
    while (true) {
        cout << "Write address and port of server to connect (or exit)" << endl;
        cout << "Example: 127.0.0.1:8888" << endl;
        string s;
        getline(cin, s);
        if (cin.eof()) {
            break;
        }
        if (s == "exit") {
            break;
        }
        size_t del_pos = s.find(":");
        if (del_pos == string::npos) {
            cout << "Wrong format" << endl;
            continue;
        }
        string addr = s.substr(0, del_pos);
        string port = s.substr(del_pos + 1);
        if (addr.empty() || port.empty()) {
            cout << "Wrong format" << endl;
            continue;
        } else {
            resolve(addr, port);
        }
    }
}

