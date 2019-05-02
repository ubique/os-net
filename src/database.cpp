#include "database.h"

Database::Database() {}

void Database::add(std::string const &uid, std::string const &account,
                   std::string const &password)
{
    if (data.find(uid) == data.end()) {
        data[uid] = {};
    }
    data[uid][account] = password;
}
bool Database::check_uid(std::string const &uid)
{
    return data.find(uid) != data.end();
}

bool Database::check_account(std::string const &uid, std::string const &account)
{
    return check_uid(uid) && data[uid].find(account) != data[uid].end();
}
bool Database::check_password(std::string const &uid,
                              std::string const &account,
                              std::string const &password)
{
    return check_account(uid, account) && data[uid][account] == password;
}
