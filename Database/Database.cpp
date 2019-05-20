//
// Created by Noname Untitled on 20.05.19.
//

#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "../Person/Person.h"

#include "Database.h"

void Database::insert(const Person &person, const std::string &number) {
    mPersonToNumbers[person].push_back(number);
}

std::stringstream Database::printAll() const {
    std::stringstream result;

    for (const auto &mPersonToNumber : mPersonToNumbers) {
        result << mPersonToNumber.first << ": ";

        for (const std::string &number : mPersonToNumber.second) {
            result << number << " ";
        }

        result << std::endl;
    }

    return result;
}

std::stringstream Database::printPerson(const Person &person) const {
    std::stringstream result;

    result << person << ": ";

    if (mPersonToNumbers.count(person)) {
        for (const std::string &number : mPersonToNumbers.at(person)) {
            result << number << " ";
        }
    } else {
        result << "No numbers found.";
    }

    result << std::endl;

    return result;
}
