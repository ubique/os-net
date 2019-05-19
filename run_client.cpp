#include "client.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Expected 2 arguments: <address> <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    try {
        client client(argv[1], static_cast<uint16_t>(std::stoul(argv[2])));
        std::cout << "Print your message to get echo or <ex> to stop the program" << std::endl;
        while (!std::cin.eof()) {
            std::string message;
            getline(std::cin, message);
            if (message == "ex") {
                break;
            }

            std::cout << "Echo: " << client.make_request(message) << std::endl;
        }
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid port" << std::endl;
        exit(EXIT_FAILURE);
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}