//
// Created by Кирилл Чулков on 2019-05-11.
//

#ifndef OS_LIB_MESSAGE_H
#define OS_LIB_MESSAGE_H

#include <string>
#include <cstddef>

class Message {
public:
    explicit Message(std::string text = "", bool isDel = false);

    bool isDeleted();
    void deleted();
    size_t getSize() const;
    std::string& getText();

private:
    std::string text;
    bool isDel;
    size_t size;
};


#endif //OS_LIB_MESSAGE_H
