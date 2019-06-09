#include <iostream>
#include <regex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFF_SIZE = 1024;
const int PORT = 8080;
const char* IP = "127.0.0.1";


void show_usage() {
    printf("Usage: [port] [IPv4]\n"
           "by default values are\n"
           "\tPort: 8080\n"
           "\tAddress: 127.0.0.1\n\n");
}


/*int open_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        show_err("Cannot open socket");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}


void start(int socket, int port, int address, std::string &message) {
    sockaddr_in serverAddress{};

    serverAddress.sin_addr.s_addr = address;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_family = AF_INET;

    if (connect(socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0) {
        show_err("Cannot connect client");
        exit(EXIT_FAILURE);
    }

    const char* messageData = message.c_str();

    size_t messageSize = strlen(messageData);
    if (send(socket, messageData, messageSize, 0) != messageSize) {
        show_err("Cannot send message");
        exit(EXIT_FAILURE);
    }

    char reply[BUFF_SIZE] = {};
    if (read(socket, reply, BUFF_SIZE) < 0) {
        show_err("Cannot read");
        exit(EXIT_FAILURE);
    }
    printf("Server reply");

    printf("Server reply: %s\n", reply);
}

int main(int argc, char** argv) {    
    if (argc > 2) {
        show_usage();
    }

    int port = PORT;

    int address = inet_addr(IP);

    std::string request = "Hello, World!";

    switch (argc) {
        case 4:
            request = argv[3];
        case 3: 
            address = inet_addr(argv[2]);
        case 2: 
            port = atoi(argv[1]);
            break;
        default:
            break;
    }
    
    int socket = open_socket();

    start(socket, port, address, request);

    close(socket);

    exit(EXIT_SUCCESS);
}*/


int parse_input(in_port_t &port, struct in_addr &ip, char **in, int const &size) {
    if (size > 3) {
        std::cerr << "Too much arguments\n";
        return -1;
    }

    try {
        port = (size == 1) ? PORT : std::atof(in[1]);
    } catch (std::exception const &e) {
        perror("Incorrect port");
        return -1;
    }

    int s = inet_pton(AF_INET, (size == 3) ? in[2] : IP, &ip);
    if (s <= 0) {
        if (s == 0)
            std::cerr << "Not in presentation IPv4 format\n";
        else
            perror("Incorrect IPv4 format");
        return -1;
    }

    return 0;
} 

int main (int argc, char **argv) {
    in_port_t port;
    struct in_addr ip;


    if (parse_input(port, ip, argv, argc) < 0) {
        show_usage();
        exit(1);
    }
   
    std::cout << "Client started..." << std::endl;

    int sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Cannot open socket");
        exit(2);
    }

    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_addr = ip;

    std::cout << "Connecting..." << std::endl;

    if (connect(sock, (struct sockaddr *) &SockAddr, sizeof(SockAddr)) < 0) {
        perror("Cannot connect");
        close(sock);
        exit(3);
    }

    std::cout << "Connect!" << std::endl;
    
    std::string message;
    std::cout << "Write your message: ";
    while (getline(std::cin, message)) {        
        if (strcmp(message.c_str(), "exit") == 0) {
            shutdown(sock, SHUT_RDWR);
            close(sock);
            exit(EXIT_SUCCESS);
        }
        
        if (strlen(message.data()) >= BUFF_SIZE) {
            std::cerr << "Too long message\n";
            continue;
        }
        message += -1;
        std::cout << "Sending a request..." << std::endl;

        std::string request;
        int sent = 0;
        do {
            request = message.substr(sent, message.size());
            int send_bytes = send(sock, request.c_str(), request.size(), 0);
            if(send_bytes <= 0) {
                perror("Cannot send response");
                shutdown(sock, SHUT_RDWR);
                close(sock);
                exit(4);
            }
            sent += send_bytes;
        } while(sent < message.size());

        std::cout << "Request sent!" << std::endl;
      
        if (strcmp(message.substr(0, message.size() - 1).c_str(), "stop") == 0) {
            std::cout << "Server stoped..." << std::endl;
            std::cout << "Good bye!" << std::endl;
            shutdown(sock, SHUT_RDWR);
            close(sock);
            exit(EXIT_SUCCESS);
        }
 

        std::cout << "Receipt of a response..." << std::endl;

        char buffer[BUFF_SIZE];
        int read = 0;
        std::string response;
        do {            
            read = recv (sock, buffer, BUFF_SIZE, MSG_NOSIGNAL);
            if (read < 0) {
                perror("Cannot receive response");
                shutdown(sock, SHUT_RDWR);
                close(sock);
                exit(5);
            }    
            response += std::string(buffer).substr(0, read);           
        }  while (buffer[read - 1] != -1);   

        std::cout << "---------------------------------------------------------------------" << std::endl;
        std::cout << "Received response: " << response.substr(0, response.size() - 1) << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;

        std::cout << "Write your message: ";
    }

}