//
// Created by rsbat on 5/12/19.
//

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "fd_wrapper.h"
#include <vector>
#include <fstream>

static const int BUFFER_SIZE = 4096;
using std::string;
using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;


void print_error_message(const string &comment) {
    cout <<  comment << ": " << strerror(errno) << endl;
}

std::string get_error_message(const string &comment) {
    return comment + ": " + strerror(errno);
}

stringstream get_multiline_message(int socket_fd, char *buffer) {
    stringstream stream;
    ssize_t last = 0;

    bool is_newline = true; // have \n
    bool is_dot = false; // have \n.
    bool message_ended = false;

    while (!message_ended) {
        memset(buffer, 0, BUFFER_SIZE);
        last = read(socket_fd, buffer, BUFFER_SIZE);
        if (last == -1) {
            throw std::runtime_error(get_error_message("Could not read from socket"));
        }

        for (size_t i = 0; i < last; i++) {
            char ch = buffer[i];
            if (is_dot) {
                if (ch == '\r') {
                    continue;
                }
                if (ch == '\n') {
                    message_ended = true;
                }
                is_dot = false;
                is_newline = false;
            }
            if (is_newline) {
                if (ch == '.') {
                    is_dot = true;
                    is_newline = false;
                    continue;
                }
                is_newline = false;
            }
            if (ch == '\n') { is_newline = true; }
            if (!message_ended && ch != '\r') {
                stream.put(ch);
            }
        }
    }

    return stream;
}

string get_message(int socket_fd, char *buffer) {
    stringstream stream;
    ssize_t last = 0;

    do {
        memset(buffer, 0, BUFFER_SIZE);
        last = read(socket_fd, buffer, BUFFER_SIZE);
        if (last == -1) {
            throw std::runtime_error(get_error_message("Could not read from socket"));
        }

        stream.write(buffer, last);
    } while (buffer[last - 1] != '\n');

    return stream.str();
}

void send_message(int socket_fd, const string &message) {
    if (write(socket_fd, message.c_str(), message.size()) == -1) {
        throw std::runtime_error(get_error_message("Could not write to socket"));
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage client address port" << endl;
        return 0;
    }
    string address = argv[1];
    string port = argv[2];

    cout << "Connecting" << endl;

    Fd_wrapper socket_fd{socket(AF_INET, SOCK_STREAM, 0)};

    if (socket_fd == -1) {
        cout << "Could not create socket: " << strerror(errno) << endl;
        return 0;
    }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(static_cast<uint16_t>(std::stoi(port)));

    int pton_status = inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr);
    if (pton_status == 0) {
        print_error_message(address + " is not a valid address");
        return 0;
    } else if (pton_status == -1) {
        print_error_message("AF_INET is not supported");
        return 0;
    }

    if (connect(socket_fd,
            reinterpret_cast<sockaddr*>(&serv_addr),
            sizeof(sockaddr_in)) == -1) {
        print_error_message("Could not connect to server");
        return 0;
    }

    char buffer[BUFFER_SIZE];

    cout << "Connected" << endl;
    cout << get_message(socket_fd, buffer);

    cout << "Input username:" << endl;
    memset(buffer, 0, BUFFER_SIZE);
    cin.getline(buffer, 256);
    send_message(socket_fd, "user " + string(buffer) + "\r\n");

    cout << get_message(socket_fd, buffer);

    cout << "Input password:" << endl;;
    memset(buffer, 0, BUFFER_SIZE);
    cin.getline(buffer, 256);
    send_message(socket_fd, "pass " + string(buffer) + "\r\n");

    cout << get_message(socket_fd, buffer);

    cout << "Getting mail stat" << endl;
    send_message(socket_fd, "stat\r\n");
    cout << get_message(socket_fd, buffer);

    cout << "Getting mail list" << endl;
    send_message(socket_fd, "list\r\n");

    stringstream list_response_stream = get_multiline_message(socket_fd, buffer);
    cout << list_response_stream.str();

    list_response_stream.getline(buffer, BUFFER_SIZE); // ignore first line
    int message_id;
    int message_size;
    while (list_response_stream >> message_id) {
        list_response_stream >> message_size;
        cout << "Downloading message #" << message_id << " to file 'letter" << message_id  << "'" << endl;

        string download_request = "retr " + std::to_string(message_id) + "\r\n";
        write(socket_fd, download_request.c_str(), download_request.size());
        stringstream retr_response_stream = get_multiline_message(socket_fd, buffer);

        std::ofstream file_stream("letter" + std::to_string(message_id));

        retr_response_stream.getline(buffer, BUFFER_SIZE); // ignore first line
        file_stream << retr_response_stream.rdbuf();

        cout << "Downloaded" << endl;
    }

    cout << "Logging out" << endl;
    send_message(socket_fd, "quit\r\n");

    cout << get_message(socket_fd, buffer);

    socket_fd.close();

    return 0;
}