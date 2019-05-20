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
    if(connect(clientSocket, ( struct sockaddr*) (&addr), sizeof(addr)) == -1){
        std::cerr<<"Error occured while creating the connection between socket and client"<<"\n";
        exit(EXIT_FAILURE);
    }
    auto message = argv[2];
    if(send(clientSocket,argv[2],strlen(argv[2]),0) == -1){
        std::cerr<<"Error occured while sending the message to server"<<"\n";
        exit(EXIT_FAILURE);
    }
    if(recv(clientSocket,response.data(),response.size(),0) == -1){
        std::cerr<<"Error occured while receiving the message from server"<<"\n";
        exit(EXIT_FAILURE);
    }
    std::cout<<"Result: "<<response.data()<<"\n";
    if(strncmp(response.data(),"exit",response.size()) == 0){
        std::cout<<"Quitting from the server"<<"\n";
    }
    if(close(clientSocket) == -1){
        std::cerr<<"Error occured while closing the client socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}