#include <utility>

//
// Created by rsbat on 5/13/19.
//

#ifndef OS_NET_MAIL_DB_H
#define OS_NET_MAIL_DB_H

#include <string>
#include <vector>

struct mail_info {
    std::string filename;
    size_t size;
    bool is_deleted;

    mail_info(std::string filename, size_t size, bool is_deleted) : filename(std::move(filename)), size(size), is_deleted(is_deleted) {};
};

class Mail_DB {
public:
    explicit Mail_DB(std::string const& mail_dir);

    bool open_inbox(std::string const& user); //THINK return more information
    void close_inbox();
    std::pair<size_t , size_t > stat();
    std::vector<std::pair<size_t , size_t >> list();
    std::pair<size_t , size_t> list(size_t n);
    std::string retr(size_t n);
    bool dele(size_t n); //THINK return more information
    void rset();
    void apply_delete();
private:
    std::string user;
    std::string mail_dir;
    std::vector<mail_info> base;

    size_t calc_size(std::string const& filename);
};


#endif //OS_NET_MAIL_DB_H
