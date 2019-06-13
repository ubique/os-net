#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <unistd.h>

constexpr  int BUFFER = 512;
int main(int argc , char* argv[]){
    if(argc != 3 ){
        std::cerr<<"Wrong format of input. Expected : [port [message]]"<<"\n";
        exit(EXIT_FAILURE);
    }
    int clientSocket;
    if((clientSocket = socket(AF_INET,SOCK_STREAM,0)) == -1){
        std::cerr<<"Error occured while creating the socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (atoi(argv[1]));
    std::vector<char> response(BUFFER);
    memset(response.data(),0,BUFFER);
    if(connect(clientSocket, ( struct sockaddr*) (&addr), sizeof(addr)) == -1){
        std::cerr<<"Error occured while creating the connection between socket and client"<<"\n";
        exit(EXIT_FAILURE);
    }
    std::string message = argv[2];
    for(;;) {
        size_t result = 0;
        while (result < message.size()) {
            int bytes = send(clientSocket, message.data() + result, message.size() - result, 0);
            if (bytes == -1) {
                std::cerr << "Error occured during sending the message";
                exit(EXIT_FAILURE);
            } else if (bytes == 0) {
                break;
            }
            result += bytes;
        }
        // receiving msg itself
        size_t sizeOfMessage = 0;
        size_t receivedBytesMessage = 0;
        while (receivedBytesMessage < message.size()) {
            receivedBytesMessage = recv(clientSocket, response.data() + sizeOfMessage, response.size() - sizeOfMessage,
                                        0);
            if (receivedBytesMessage == -1) {
                std::cerr << "Error occured during getting the message";
                exit(EXIT_FAILURE);
            }
            sizeOfMessage += receivedBytesMessage;
        }
        std::cout << "Result: " << response.data() << "\n";
        if (strncmp(response.data(), "exit", response.size()) == 0) {
            std::cout << "Quitting from the server" << "\n";
            break;
        }
        std::cin >> message;
        memset(response.data(),0,BUFFER);
    }
    if(close(clientSocket) == -1){
        std::cerr<<"Error occured while closing the client socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}