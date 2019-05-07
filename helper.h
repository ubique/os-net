//
// Created by max on 01.05.19.
//

#pragma once

#include <iostream>

static constexpr size_t BUFFER_SIZE = 20;

void prerror(const char *msg);

std::string prerror_str(const char *msg);

void checker(int, const char *, int = - 1);
