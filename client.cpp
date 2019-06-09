#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

void print_usage() {
    cout << "Usage: client [address] [port] [message]" << endl;
}

void close_socket(int fd) {
    if (close(fd) < 0) {
        perror("Cannot close socket");
    }
}

const int BUFF_SIZE = 1024;
const int MAX_LEN = 100;


int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage();
        return 0;
    }

    int len = strlen(argv[3]);
    if (len > MAX_LEN) {
        cout << "Message must be shorter than 100";
        exit(EXIT_FAILURE);
    }
    char *message = new char[len + 1];
    strcpy(message, argv[3]);
    message[len] = '\n';
    int sock = 0;
    struct sockaddr_in server_address;
    int port;
    try {
        port = stoull(argv[2]);
    } catch (exception &e) {
        cout << "Invalid port: " << argv[2] << endl;
        exit(EXIT_FAILURE);
    }

    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        cout << "Invalid address: " << argv[1] << '\n';
        close_socket(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close_socket(sock);
        exit(EXIT_FAILURE);
    }

    int length = strlen(message), sent = 0;
    bool err = false;
    while (sent < length && !err) {
        cout.flush();
        int cnt = send(sock, message + sent, length - sent, 0);
        if (cnt < 0) {
            err = true;
        }
        sent += cnt;
    }
    if (err) {
        perror("Sending failed");
        close_socket(sock);
        exit(EXIT_FAILURE);
    }
    cout << "Sent: " << message;
    char buffer[BUFF_SIZE] = {0};
    int valread, received = 0;
    while ((valread = recv(sock, buffer + received, BUFF_SIZE - received, 0)) > 0) {
        received += valread;
        if (buffer[received - 1] == '\n') {
            break;
        }
    }
    if (valread < 0) {
        perror("Receiving failed");
        close_socket(sock);
        exit(EXIT_FAILURE);
    }
    cout << "Received: " << buffer;
    close_socket(sock);
    return 0;

}