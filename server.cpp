#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <cstring>
#include <unistd.h>



constexpr  int BUFFER = 1024;
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

    if(listen(socket_descriptor,1) == -1){
        std::cerr<<"Error occured while setting listening a socket"<<"\n";
        exit(EXIT_FAILURE);
    }
    std::vector<char> response(BUFFER);
    bool on = false;
    for(;;){
        if(on) break;
        memset(response.data(),0,BUFFER);
        int other;
        if((other = accept(socket_descriptor, nullptr,nullptr)) == -1){
            std::cerr<<"Error occured while receiving request from client";
            continue;
        }
        std::cout<<"Client is connected"<<"\n";
        size_t receivedBytes = 0;
        char data[BUFFER];
        memset(data,0,BUFFER);
        for(;;){
            receivedBytes = recv(other,data,BUFFER,0);
            if(receivedBytes == -1){
                std::cerr<<"Error occured during receving the message of size : " << BUFFER << "\n";
                continue;
            }
            if(receivedBytes == 0){
                break;
            }
            size_t sizeOfMessage = 0;
            size_t receivedBytesMessage = 0;
            std::cout<<"Message(or part of it)\"" << data << "\" is received"<<"\n";
            std::cout<<"Sending message to client...."<<"\n";
            while(receivedBytesMessage < receivedBytes){
                size_t sended = send(other,data + receivedBytesMessage, receivedBytes - receivedBytesMessage,0);
                if(sended == -1){
                    std::cerr<<"Error occured during getting the message";
                    exit(EXIT_FAILURE);
                }
                receivedBytesMessage += sended;
            }
            if(strncmp(data,"exit",4) == 0){
                std::cout<<"Exiting the server... Wait for the free port before next use"<<"\n";
                if(close(other) == -1){
                    std::cerr<<"Error occured while closing socket";
                }
                on = true;
                break;
            }
            memset(data,0,BUFFER);
        }
    }
    if(close(socket_descriptor) == -1){
        std::cerr<<"Error occured while closing the server"<<"\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}