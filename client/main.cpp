//
// Created by vitalya on 19.05.19.
//

#include "client.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Address argument expected" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        client client(argv[1]);
        std::cout << "This is echo client. Send something to the server and it will return back.\n"
                     "Send \"SHUTDOWN\" to shutdown server. Type \"exit\" to exit." << std::endl;
        std::string message;
        while (true) {
            std::cin >> message;
            if (message == "exit") {
                break;
            }
            std::string ret = client.send(message);

            if (message != ret) {
                std::cout << "Oh, sh#t!" << std::endl;
                return EXIT_FAILURE;
            }
        }
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
