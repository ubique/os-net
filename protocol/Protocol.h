#pragma once

#include <vector>

namespace net {

    class Protocol {
    public:
        virtual bool sender(const int& descriptor, const std::vector<char>& data) const = 0;

        virtual bool receiver(const int& descriptor) const = 0;

    };
}


