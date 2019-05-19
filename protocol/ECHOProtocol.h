#pragma once

#include "Protocol.h"
#include "../utils/Utils.h"

namespace net {
    class ECHOProtocol : public Protocol {
    public:
        bool sender(const int& descriptor, const data_t& data) const override;

        bool receiver(const int& descriptor) const override;

    private:

        bool read(const int& descriptor,  data_t& receivedData, const int& targetSize) const;
    };
}

