//
// Created by Кирилл Чулков on 2019-05-11.
//

#include "Message.h"

Message::Message(std::string text, bool isDel) : text(std::move(text)), isDel(isDel), size(text.size()) {}

bool Message::isDeleted() {
    return isDel;
}

void Message::deleted() {
    isDel = true;
}

std::string& Message::getText() {
    return text;
}

size_t Message::getSize() const {
    return this->size;
}
