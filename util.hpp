#pragma once

#include <eggs/variant.hpp>
#include <unistd.h>
#include <type_traits>
#include <vector>


template<typename R>
using Result = eggs::variant<R, char const*>;

struct defered_close
{
    explicit defered_close(int fd)
        : fd(fd)
    {}

    defered_close(defered_close&& other)
        : fd(other.fd)
    {
        other.fd = -1;
    }

    defered_close& operator=(defered_close&& other)
    {
        fd = other.fd;
        other.fd = -1;
        return *this;
    }

    ~defered_close() noexcept
    {
        if (fd == -1) return;
        close(fd);
    }

private:
    defered_close(defered_close const& other) = delete;
    defered_close& operator=(defered_close const& other) = delete;

private:
    int fd;
};

template<typename InputIterator,
         typename = std::enable_if_t<
                        std::is_same<
                            std::decay_t<
                                decltype ((*std::declval<InputIterator>()))
                            >,
                            int
                        >::value
                    >
         >
static inline std::vector<defered_close> defer_closev(InputIterator begin,
                                                      InputIterator end)
{
    std::vector<defered_close> result;
    while (begin != end)
    {
        result.emplace_back(defer_close(*begin));
        ++begin;
    }

    return result;
}
