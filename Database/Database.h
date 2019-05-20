//
// Created by Noname Untitled on 20.05.19.
//

#pragma once

class Database {
public:
    Database() = default;

    void insert(const Person &, const std::string &);

    std::stringstream printAll() const;

    std::stringstream printPerson(const Person&) const;

private:
    std::map<Person, std::vector<std::string>> mPersonToNumbers;
};
