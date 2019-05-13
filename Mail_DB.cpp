//
// Created by rsbat on 5/13/19.
//

#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Mail_DB.h"
#include "fd_wrapper.h"

Mail_DB::Mail_DB(std::string const& mail_dir) : mail_dir(mail_dir) {
    if (this->mail_dir.back() != '/') {
        this->mail_dir += '/';
    }
}

/**
 * Initializes DB
 * Assumes there are no folders in inbox folder
 * @param user
 * @return
 */
bool Mail_DB::open_inbox(std::string const& user) {
    close_inbox();
    this->user = user;
    std::string dir_name = mail_dir + user;
    DIR* dir = opendir(dir_name.c_str());

    if (dir == nullptr) {
        //TODO
        return false;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename == "." || filename == "..") { continue; }

        base.emplace_back(filename, calc_size(filename), false);
    }

    closedir(dir);

    return true;
}

/**
 * Calculates size according to POP3 standart
 * Assumes server runs on Linux
 * Adds one byte for each \n because it will be replaced with \r\n
 * @param filename
 * @return
 */
 //THINK more efficient
size_t Mail_DB::calc_size(std::string const &filename) {
    std::cerr << mail_dir << user << "/" << filename << std::endl;
    std::ifstream file(mail_dir + user + "/" + filename);
    size_t size = 0;

    int ch;

    while ((ch = file.get()) != std::ifstream::traits_type::eof()) {
        if (ch == '\n') { size++; }
        size++;
    }

    std::cerr << size << std::endl;

    return size;
}

void Mail_DB::close_inbox() {
    user.clear();
    base.clear();
    //TODO
}

std::pair<size_t, size_t> Mail_DB::stat() {
    size_t total_size = 0;
    size_t count = 0;
    for (const auto &info : base) {
        if (!info.is_deleted) {
            total_size += info.size;
            count++;
        }
    }

    return {count, total_size};
}

std::vector<std::pair<size_t, size_t >> Mail_DB::list() {
    std::vector<std::pair<size_t, size_t>> result;

    for (size_t i = 0; i < base.size(); i++) {
        if (!base[i].is_deleted) {
            result.emplace_back(i + 1, base[i].size);
        }
    }
    return result;
}

//TODO check if deleted
std::pair<size_t, size_t> Mail_DB::list(size_t n) {
    return {n + 1, base[n].size};
}

//TODO check if deleted
std::string Mail_DB::retr(size_t n) {
    std::string result;
    std::ifstream file(mail_dir + user + "/" + base[n].filename);
    bool newline = true;

    int ch;
    while ((ch = file.get()) != std::ifstream::traits_type::eof()) {
        if (ch == '\n') { result += '\r'; }
        if (ch == '.' && newline) { result += '.'; }
        result += static_cast<char>(ch);
        newline = ch == '\n';
    }
    result += ".\r\n";
    return result;
}

//TODO check if deleted
bool Mail_DB::dele(size_t n) {
    base[n].is_deleted = true;
    return true;
}

void Mail_DB::rset() {
    for (auto& info : base) {
        info.is_deleted = false;
    }
}

void Mail_DB::apply_delete() {
    for (const auto &info : base) {
        if (info.is_deleted) {
            int result = unlink((mail_dir + user + "/" + info.filename).c_str());
            if (result == -1) {
                std::cerr << "Could not delete email at " << mail_dir << user << "/" << info.filename
                    << ": " << strerror(errno) << std::endl;
            }
        }
    }
}
