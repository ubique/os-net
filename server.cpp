#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <cstring>
#include <unistd.h>

constexpr  int BUFFER = 512;
int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cerr << "Wrong format of input. Expected : [port]" << "\n";
        exit(EXIT_FAILURE);
    }

    int socket_descriptor;
    if((socket_descriptor = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        std::cerr << "Error occured while creating the socket" << "\n";
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(socket_descriptor, reinterpret_cast<sockaddr*> (&addr),sizeof(addr)) == -1){
        std::cerr<<"Error occured while binding to socket"<<"\n";
        exit(EXIT_FAILURE);
    }

    if(listen(socket_descriptor,1)==-1){
        std::cerr<<"Error occured while setting listening a socker"<<"\n";
        exit(EXIT_FAILURE);
    }
    std::vector<char> response(BUFFER);
    for(;;){

        int other;
        if((other = accept(socket_descriptor, nullptr,nullptr)) == -1){
            std::cerr<<"Error occured while receiving request from client";
            continue;
        }
        ssize_t result;
        if((result = recv(other,response.data(),BUFFER,0)) == -1 ){
            std::cerr<<"Error ocuured while receiving a message"<<"\n";
            continue;
        }
        if(strncmp(response.data(),"exit",result) == 0){
            std::cout<<"Exiting the server... Wait for the free port before next use"<<"\n";
            if(close(other) == -1){
                std::cerr<<"Error occured while closing socket";
            }
            break;
        }
        if(send(other,response.data(),result,0) == -1){
            std::cerr<<"Error occuted while sending the response"<<"\n";
            continue;
        }
    }
    if(close(socket_descriptor) == -1){
        std::cerr<<"Error occured while closing the server"<<"\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}