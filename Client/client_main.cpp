//
// Created by Noname Untitled on 19.05.19.
//

#include <iostream>

#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Client.h"

void showUsage(const char *filename) {
    std::cerr << "Usage: " << filename << " <address> <port> <command> [message]" << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        std::cerr << "Wrong arguments. ";
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    std::string message;
    for (int j = 3; j < argc; ++j) {
        if (j != 3) {
            message += " ";
        }
        message += argv[j];
    }

    std::cout << "Message to server: " << message << std::endl;

    Client myClient = Client(argv[1], argv[2], message);

    myClient.createConnection();

    myClient.sendInfo();

    char *response = myClient.getResponse();

    std::cout << "Response from server: " << response << std::endl;

    exit(EXIT_SUCCESS);

}
