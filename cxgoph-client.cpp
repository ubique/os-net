#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

const size_t BUFFER_SIZE = 4096;


void print_usage() {
    std::cerr << R"(cxgoph - easy Gopher server and client implementation in C++

Usage: ./cxgoph-client [-h] | host [[port] path]

Arguments:
    -h          Prints the help message and exits.
    host        Sets the host to listen on.
                (required)
    port        Sets the port to listen on.
                (default: 70)
    path        Sets the path to retrieve.
                (default: .))" << std::endl;
}


class sockoutbuf: public std::streambuf {
private:
    int fd;
protected:
    virtual int overflow(int c) {
        if (c != EOF) {
            if (send(fd, &c, 1, 0) <= 0) {
                return EOF;
            }
        }
          
        return c;
    }
public:
    sockoutbuf(int fd): std::streambuf(), fd(fd) {}
};


void print_error(const char* reason) {
    std::cerr << "cxgoph-client: " << reason;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << std::endl;
}


int main(int argc, char *argv[]) {
    std::string host;
    int port = 70;
    std::string path = ".";

    if (argc == 2) {
        if (std::string(argv[1]) == "-h") {
            print_usage();
            return EXIT_SUCCESS;
        }

        host = argv[1];
    } else if (argc == 3) {
        host = argv[1];
        path = argv[2];
    } else if (argc == 4) {
        host = argv[1];
        port = std::atoi(argv[2]);
        path = argv[3];
    } else {
        print_usage();
        return EXIT_FAILURE;
    }

    if (port == 0) {
        print_error("Invalid port number");
        return EXIT_FAILURE;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        print_error("Can't create socket");
        return EXIT_FAILURE;
    }

    {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            print_error("Invalid host");
            return EXIT_FAILURE;
        }

        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            print_error("Can't connect to server");
            return EXIT_FAILURE;
        }
    }

    path += "\r\n";

    {
        size_t sent = 0;

        while (sent < path.size()) {
            int sent_packet = send(sock, path.data() + sent, path.size() - sent, 0);
            if (sent_packet < 0) {
                print_error("Can't send request");
                return EXIT_FAILURE;
            }
            sent += sent_packet;
        }
    }

    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        int status;

        while ((status = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
            std::cout << buffer;
            memset(buffer, 0, sizeof(buffer));
        }

        if (status < 0) {
            print_error("Can't receive response");
            return EXIT_FAILURE;
        }
    }

    close(sock);

    return EXIT_SUCCESS;
}
