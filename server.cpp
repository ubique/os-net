#include <iostream>
#include <vector>
#include <array>
#include <unistd.h>
#include <cstring>
#include <string>
#include <netinet/in.h>

const int BUFF_SIZE = 1025;
const int PORT = 8080;

void show_usage() {
    printf("Usage: server [port]\n"
           "by default port is 8080\n\n");
}

int parse_input(int &port, char **in, int size) {
    if (size > 2) {
        std::cerr << "Too much arguments\n";
        return -1;
    }
    try {
        port = (size == 1) ? PORT : std::stoi(in[1]);
    }
    catch (std::exception const &e) {
        perror("Incorrect port");
        return -1;
    }
    return 0;
}

// serever [port];

int main(int argc, char **argv) {
    int port;

    if (parse_input(port, argv, argc) < 0) {
        show_usage();
        exit(1);
    }

    std::cout << "Server started..." << std::endl;

    int MasterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (MasterSocket < 0) {
        perror("Cannot open socket");
        exit(2);
    }

    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(MasterSocket, (struct sockaddr *)&SockAddr, sizeof(SockAddr)) < 0) {
        perror("Cannot bind");
        close(MasterSocket);
        exit(3);
    }

    if (listen(MasterSocket, SOMAXCONN) < 0) {
        perror("Cannot listen");
        close(MasterSocket);
        exit(4);
    }


    bool next = true;
    while (true) {        
        int SlaveSocket;

        if (next) {
            SlaveSocket = accept(MasterSocket, nullptr, nullptr);
            if (SlaveSocket < 0) {
            perror("Cannot accept");
            shutdown(SlaveSocket, SHUT_RDWR);
            close(SlaveSocket);
            }
        }

        std::cout << "Receipt of a request..." << std::endl;

        char buffer[BUFF_SIZE];  

        next = false;
        int read = 0;
        std::string request = "";
        do {            
            read = recv (SlaveSocket, buffer, BUFF_SIZE, 0);
            if (read <= 0) {
                perror("Cannot receive request");
                shutdown(SlaveSocket, SHUT_RDWR);
                close(SlaveSocket);
                next = true; 
                break;
            }    
            request += std::string(buffer).substr(0, read);
        } while (buffer[read - 1] != -1);
        if (next) {
            continue;
        }


        if (strcmp(request.substr(0, request.size() - 1).c_str(), "stop") == 0) {
            std::cout << "Server stoped..." << std::endl;
            std::cout << "Good Bye!" << std::endl;
            shutdown(SlaveSocket, SHUT_RDWR);
            close(SlaveSocket);
            close(MasterSocket);
            exit(EXIT_SUCCESS);
        }

        std::cout << "---------------------------------------------------------------------" << std::endl;
        std::cout << "Received request: " << request.substr(0, request.size() - 1) << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;


        std::cout << "Sending a response..." << std::endl;
        
        std::string response;
        int sent = 0;
        next = false; 
        do {
            response = request.substr(sent, request.size());
            int send_bytes = send(SlaveSocket, response.c_str(), response.size(), 0);
            if (sent < 0) {
                perror("Cannot send response");
                shutdown(SlaveSocket, SHUT_RDWR);
                close(SlaveSocket);
                next = true;
                break;
            }
            sent += send_bytes;
        } while (sent < request.size());

        if (next) 
            continue;

        std::cout << "Response sent!" << std::endl;
    }
}