#ifndef SFTP_SERVER_H
#define SFTP_SERVER_H
#include "database.h"
#include <string>

class sftp_server
{
public:
    sftp_server(std::string const &address, std::string const &port);

    sftp_server(sftp_server const &) = delete;

    void run();
    void add_account(std::string const &uid, std::string const &account,
                     std::string const &password);

    ~sftp_server();

private:
    int server_socket;
    Database database;
    std::string cwd;
};

#endif // SFTP_SERVER_H
