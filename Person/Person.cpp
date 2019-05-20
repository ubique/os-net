//
// Created by Noname Untitled on 20.05.19.
//
#include <string>
#include <cstring>
#include <sstream>

#include "Person.h"

Person::Person(const std::string &firstName, const std::string &lastName) :
        mFirstName(firstName), mLastName(lastName) {}

const std::string Person::getFirstName() const {
    return mFirstName;
}

const std::string Person::getLastName() const {
    return mLastName;
}

bool operator<(const Person &first, const Person &second) {
    if (first.getFirstName() == second.getFirstName()) {
        return first.getLastName() < second.getLastName();
    }
    return first.getFirstName() < second.getFirstName();
}

std::ostream &operator<<(std::ostream &os, const Person &person) {
    os << person.getFirstName() << " " << person.getLastName();
    return os;
}