#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>

class Database
{
public:
    Database();

    void add(std::string const &uid, std::string const &account,
             std::string const &password);
    bool check_uid(std::string const &uid);
    bool check_account(std::string const &uid, std::string const &account);
    bool check_password(std::string const &uid, std::string const &account,
                        std::string const &password);

private:
    std::map<std::string, std::map<std::string, std::string>> data;
};

#endif // DATABASE_H
