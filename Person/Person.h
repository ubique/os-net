//
// Created by Noname Untitled on 20.05.19.
//

#pragma once

class Person {
public:
    Person(const std::string &, const std::string &);

    const std::string getFirstName() const;

    const std::string getLastName() const;

private:
    const std::string mFirstName;
    const std::string mLastName;
};

bool operator<(const Person &, const Person &);

std::ostream &operator<<(std::ostream &, const Person &);