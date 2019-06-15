#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

const size_t BUFFER_SIZE = 4096;
std::string public_host;
int public_port = 0;


void print_usage() {
    std::cerr << R"(cxgoph - easy Gopher server and client implementation in C++

Usage: ./cxgoph-server [-h | [host] port]

Arguments:
    -h          Prints the help message and exits.
    host        Sets the host to listen on.
                (default: 0.0.0.0)
    port        Sets the port to listen on.
                (default: 70)

Note: listening on ports lower than 1024 requires
superuser privileges.)" << std::endl;
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
    std::cerr << "cxgoph-server: " << reason;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << std::endl;
}


void process(std::string request, std::ostream& out) {
    std::istringstream tokenizer(request);
    std::string token;

    std::string path = ".";
    
    while (std::getline(tokenizer, token, '/')) {
        if (token == "" || token == ".") {
            continue;
        }

        if (token == "..") {
            out << "3Invalid request.\r\n.";
            return;
        }

        path += "/" + token;
    }

    struct stat s;

    if (stat(path.c_str(), &s) != 0) {
        if (errno == ENOENT) {
            out << "3Resource does not exist.\r\n.";
        } else {
            out << "3Can't open file.\r\n.";
            print_error("Can't stat file");
        }
        return;
    }

    if (s.st_mode & S_IFDIR) {
        out << "7Directory listing for " << path << "\r\n";

        DIR *dir = opendir(path.c_str());
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            char type;

            if (entry->d_type == DT_DIR) {
                type = '1';
            } else if (entry->d_type == DT_REG) {
                type = '9';
            } else {
                continue;
            }

            out << type << entry->d_name << '\t' << request << '/' << entry->d_name << '\t' << public_host << '\t' << public_port << "\r\n";
        }
        closedir(dir);
        out << ".";
    } else if (s.st_mode & S_IFREG) {
        std::ifstream is(path, std::ios::binary | std::ios::in);
        if (is.fail()) {
            out << "3Can't open file.\r\n.";
            print_error("Can't open file");
            return;
        }
        out << is.rdbuf();
        is.close();
    } else {
        out << "3Unsupported file type.\r\n.";
    }
}


int main(int argc, char *argv[]) {
    std::string host = "0.0.0.0";
    int port = 70;

    if (argc == 1) {
        // Do nothing
    } else if (argc == 2) {
        if (std::string(argv[1]) == "-h") {
            print_usage();
            return EXIT_SUCCESS;
        }

        port = std::atoi(argv[1]);
    } else if (argc == 3) {
        host = argv[1];
        port = std::atoi(argv[2]);
    } else {
        print_usage();
        return EXIT_FAILURE;
    }

    if (port == 0) {
        print_error("Invalid port number");
        return EXIT_FAILURE;
    }

    {
        char* env_host = std::getenv("HOST");
        if (env_host != nullptr) {
            public_host = env_host;
        } else {
            public_host = host;
        }

        char* env_port = std::getenv("PORT");
        if (env_port != nullptr) {
            public_port = std::atoi(env_port);
        }
        if (env_port == 0) {
            public_port = port;
        }
    }
        
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        print_error("Can't create socket");
        return EXIT_FAILURE;
    }

    {
        int value = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &value, sizeof(value)) < 0) {
            print_error("Can't set socket options");
            return EXIT_FAILURE;
        }
    }
    
    {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            print_error("Invalid host");
            return EXIT_FAILURE;
        }

        if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            print_error("Can't bind to socket");
            return EXIT_FAILURE;
        }
    }

    if (listen(sock, SOMAXCONN) == -1) {
        print_error("Can't listen to socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in client_addr;
    unsigned int client_addr_len;
    int client_sock;

    std::string request;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    std::cerr << "Listening on " << host << ':' << port << std::endl;

    while (true) {
        client_sock = accept(sock, (sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            print_error("Can't accept connection from client");
            continue;
        }

        request.clear();

        int read;

        while ((read = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
            request.insert(request.end(), buffer, buffer + read);
            memset(buffer, 0, BUFFER_SIZE);
            size_t pos = request.find("\r\n");
            if (pos != std::string::npos) {
                request = request.substr(0, pos);
                break;
            }
        }

        if (read == -1) {
            print_error("Can't read from socket");
            continue;
        }

        sockoutbuf client_buf(client_sock);
        std::ostream client_out(&client_buf);
        process(request, client_out);
        shutdown(client_sock, SHUT_RDWR);
    }
}
