#include "sftp_server.h"
#include <arpa/inet.h>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

static const std::string GREETING = "+knisht SFTP Service\n";

static const size_t BUFFER_SIZE = 4096;

struct user_state {
    std::string uid;
    std::string account;
    std::string retrieving;
    std::string receiving;
    size_t receiving_size = 0;
    bool logged_in = false;
    bool disconnect = false;
};

sftp_server::sftp_server(std::string const &address, std::string const &port)
{
    char *cwd = get_current_dir_name();
    this->cwd = cwd;
    free(cwd);
    if (cwd == nullptr) {
        throw std::runtime_error("Could not retrieve cwd");
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        throw std::runtime_error(std::string{"Socket creating failed: "} +
                                 strerror(errno));
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address.data());
    server.sin_port = htons(static_cast<uint16_t>(stoi(port)));

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&server),
             sizeof(server)) == -1) {
        close(server_socket);
        throw std::runtime_error(std::string{"Binding failed: "} +
                                 strerror(errno));
    }

    if (listen(server_socket, 3) == -1) {
        close(server_socket);
        throw std::runtime_error(std::string{"Listening failed: "} +
                                 strerror(errno));
    }
}

static std::vector<std::string> parse_string(std::string const &target)
{
    std::istringstream stream(target);
    std::vector<std::string> result;
    while (stream) {
        std::string word;
        stream >> word;
        result.push_back(word);
    }
    return result;
}

struct fd_wrapper {
    int fd;
    explicit fd_wrapper(int fd) : fd(fd) {}
    fd_wrapper(fd_wrapper const &) = delete;
    ~fd_wrapper()
    {
        if (fd != -1 && close(fd) == -1) {
            std::cerr << "Error while closing fd: " << strerror(errno)
                      << std::endl;
        }
    }
};

static std::string process(std::vector<std::string> const &query,
                           user_state &state, Database &database)
{
    if (query.empty()) {
        return "-Empty query";
    }
    if (query[0] == "USER") {
        if (state.logged_in) {
            return "!" + state.uid + " logged in";
        }
        if (query.size() > 1 && database.check_uid(query[1])) {
            state.uid = query[1];
            return "+User-id valid, send accound and password";
        } else {
            return "Invalid user-id, try again";
        }
    }
    if (query[0] == "ACCT") {
        if (state.logged_in) {
            return "! Account valid, logged-in";
        }
        if (query.size() > 1 && database.check_account(state.uid, query[1])) {
            state.account = query[1];
            return "+Account valid, send password";
        } else {
            return "-Invalid account, try again";
        }
    }
    if (query[0] == "PASS") {
        if (query.size() > 1 &&
            database.check_password(state.uid, state.account, query[1])) {
            state.logged_in = true;
            return "! Logged in";
        } else {
            return "-Wrong password, try again";
        }
    }
    if (!state.logged_in) {
        return "- You are not logged in";
    }
    if (query[0] == "TYPE") {
        return "! Only binary type is supported";
    }
    if (query[0] == "LIST") {
        errno = 0;
        std::string directory_name = query.size() > 2 ? query[2] : ".";
        DIR *dir_pointer = opendir(directory_name.data());
        std::string directory_contents;
        if (dir_pointer) {
            struct dirent *dir_entry;
            while ((dir_entry = readdir(dir_pointer)) != nullptr) {
                directory_contents +=
                    std::string{"+"} + dir_entry->d_name + '\n';
            }
            closedir(dir_pointer);
        }
        if (errno != 0) {
            return std::string{"- Error:"} + strerror(errno);
        } else {
            return directory_contents;
        }
    }
    if (query[0] == "CDIR") {
        if (query.size() == 1)
            return "-Please, provide target directory";
        else if (chdir(query[1].data()) == -1) {
            return std::string{"-Can't connect to directory because: "} +
                   strerror(errno);
        } else {
            return "!Changed working dir to " + query[1];
        }
    }
    if (query[0] == "KILL") {
        if (query.size() == 1) {
            return "-Invalid command";
        } else if (remove(query[1].data()) == -1) {
            return std::string{"-Not deleted because of "} + strerror(errno);
        } else {
            return "+" + query[1] + " deleted";
        }
    }
    if (query[0] == "NAME") {
        return "- Renaming is not supported";
    }
    if (query[0] == "DONE") {
        state.logged_in = false;
        state.disconnect = true;
        return "+ Good bye, " + state.uid + "!";
    }
    if (query[0] == "RETR") {
        struct stat stat_info;
        if (query.size() == 1 || stat(query[1].data(), &stat_info) == -1) {
            return "-File not exist";
        } else {
            state.retrieving = query[1];
            std::ostringstream stream;
            stream << stat_info.st_size;
            return stream.str();
        }
    }
    if (query[0] == "SEND") {
        fd_wrapper wrapper(open(state.retrieving.data(), O_RDONLY));
        std::string result;
        if (wrapper.fd != -1) {
            char buffer[BUFFER_SIZE];
            while (true) {
                ssize_t received = read(wrapper.fd, buffer, BUFFER_SIZE);
                if (received == -1) {
                    return "-Error occurred while reading a file";
                } else if (received == 0) {
                    break;
                } else {
                    result +=
                        std::string{buffer, static_cast<size_t>(received)};
                }
            }
        } else {
            return std::string{"-Error occurred: "} + strerror(errno);
        }
        state.retrieving = "";
        return result;
    }
    if (query[0] == "STOP") {
        state.retrieving = "";
        return "+ok, RETR aborted";
    }
    if (query[0] == "STOR") {
        if (query.size() < 3 || query[1] != "OLD") {
            return "-Server supports only STOR OLD command";
        } else {
            state.receiving = query[2];
            struct stat stat_info;
            if (stat(state.receiving.data(), &stat_info) == -1) {
                return "+Will create new file";
            } else {
                return "+Will write over old file";
            }
        }
    }
    if (query[0] == "SIZE") {
        try {
            if (query.size() == 1) {
                throw std::runtime_error("");
            }
            size_t size = stoull(query[1]);
            if (size > BUFFER_SIZE) {
                return "-Not enough room, don't send it";
            } else {
                state.receiving_size = stoull(query[1]);
                return "+ok, waiting for the file";
            }
        } catch (std::runtime_error const &) {
            return "-Invalid command";
        }
    }
    return "-Unknown command";
}

void sftp_server::run()
{
    while (true) {
        if (chdir(cwd.data()) == -1) {
            throw std::runtime_error("Could not reach start directory");
        }
        struct sockaddr_in client;
        size_t socket_size = sizeof(struct sockaddr_in);
        fd_wrapper client_socket(
            accept(server_socket, reinterpret_cast<sockaddr *>(&client),
                   reinterpret_cast<socklen_t *>(&socket_size)));
        if (client_socket.fd == -1) {
            throw std::runtime_error(std::string{"accept: "} + strerror(errno));
        }
        std::cout << "Connected" << std::endl;
        char message_buffer[BUFFER_SIZE];
        if (write(client_socket.fd, GREETING.data(), GREETING.size()) == -1) {
            // so we can do nothing other than trying to accept new connections.
            std::cout << "Error while sending: " << strerror(errno)
                      << std::endl;
            continue;
        }
        user_state state;
        std::string storage;
        while (true) {
            memset(message_buffer, 0, BUFFER_SIZE);
            int read_size =
                recv(client_socket.fd, message_buffer, BUFFER_SIZE, 0);
            if (read_size == -1) {
                std::cerr << "[ERROR]: " << strerror(errno) << std::endl;
                break;
            }
            if (read_size == 0) {
                std::cout << "[INFO] Disconnected" << std::endl;
                break;
            }
            if (state.receiving_size != 0) {
                // we are in the state of accepting new file
                size_t write_size = std::min(static_cast<size_t>(read_size),
                                             state.receiving_size);
                state.receiving_size -= write_size;

                storage += std::string{message_buffer, write_size};
                if (state.receiving_size != 0) {
                    continue;
                }
            }
            std::string response;
            if (state.receiving_size == 0 && storage.size() != 0) {
                fd_wrapper new_file(open(state.receiving.data(),
                                         O_CREAT | O_RDWR, S_IRUSR | S_IWUSR));
                if (new_file.fd == -1 ||
                    write(new_file.fd, storage.data(), storage.size()) == -1) {
                    response = "-Could not save " + state.receiving + ": " +
                               strerror(errno);
                } else {
                    response = "+Saved " + state.receiving;
                }
                storage.clear();
                state.receiving.clear();
            } else {
                std::cout << "[INFO] Received:" << message_buffer << std::endl;
                std::vector<std::string> queries = parse_string(message_buffer);
                response = process(queries, state, database);
            }
            if (state.disconnect || write(client_socket.fd, response.data(),
                                          response.size()) == -1) {
                break;
            }
        }
    }
}

void sftp_server::add_account(std::string const &uid,
                              std::string const &account,
                              std::string const &password)
{
    database.add(uid, account, password);
}

sftp_server::~sftp_server()
{
    if (server_socket != -1) {
        close(server_socket);
    }
}
