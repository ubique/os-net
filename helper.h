//
// Created by max on 01.05.19.
//

#pragma once

#include <iostream>
#include <functional>

#define UNUSED __attribute__((unused))

static constexpr size_t BUFFER_SIZE = 20;

void prerror(const char *msg);

std::string prerror_str(const char *msg);

void checker(int, const char *, int = -1);

void checker(int ret, const std::string &msg, int error_code = -1);


class Printer {
public:
    enum class Symbols {
        End, Tab, R
    };
private:
    static std::ostream &determ(std::ostream &stream, const Symbols symbol) {
        switch (symbol) {
            case Symbols::End: {
                stream << std::endl;
                break;
            }
            case Symbols::Tab: {
                stream << '\t';
                break;
            }
            case Symbols::R: {
                stream << '\r';
                break;
            }
        }
        return stream;
    }

public:
    template<typename T>
    static std::ostream &print(std::ostream &stream, UNUSED T &&arg0) {
#ifdef DEBUG
        stream << arg0;
#endif
        return stream;
    }

    static std::ostream &print(std::ostream &stream, UNUSED Symbols &&arg0) {
#ifdef DEBUG
        determ(stream, arg0);
#endif
        return stream;
    }

    template<typename T, class... Args>
    static std::ostream &print(std::ostream &stream, UNUSED T &&arg0, UNUSED Args &&... args) {
#ifdef DEBUG
        stream << arg0;
        return print(stream, std::forward<Args>(args)...);
#else
        return stream;
#endif
    }

    template<typename T, class... Args>
    static std::ostream &print(std::ostream &stream, UNUSED Symbols &&arg0, UNUSED Args &&... args) {
#ifdef DEBUG
        return print(determ(stream, arg0), std::forward<Args>(args)...);
#else
        return stream;
#endif
    }

    template<typename T>
    static std::ostream &printr(std::ostream &stream, T &&arg0) {
        stream << arg0;
        return stream;
    }

    static std::ostream &printr(std::ostream &stream, Symbols &&arg0) {
        determ(stream, arg0);
        return stream;
    }

    template<typename T, class... Args>
    static std::ostream &printr(std::ostream &stream, T &&arg0, Args &&... args) {
        stream << arg0;
        return printr(stream, std::forward<Args>(args)...);
    }

    template<typename T, class... Args>
    static std::ostream &printr(std::ostream &stream, Symbols &&arg0, Args &&... args) {
        return printr(determ(stream, arg0), std::forward<Args>(args)...);
    }
};