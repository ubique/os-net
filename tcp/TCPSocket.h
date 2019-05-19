#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../utils/Utils.h"

namespace net {
    namespace tcp {

        class TCPSocket {
        public:
            TCPSocket(std::string ipv4, const std::string& port);

            /**
             * Function create socket and call {@link TCPSocket::afterCreate} for ip:port
             * @return true if was created successfully, false otherwise
             */
            virtual bool create();

            virtual void run(const data_t& data = data_t()) = 0;

        protected:

            int getFileDescriptor();

            virtual bool afterCreate(const sockaddr_in& in) = 0;


            std::string fullAddress() {
                return ipv4 + ":" + std::to_string(port);
            }

        private:


            /**
             * Connection file descriptor
             */

            int fileDescriptor;

            /**
             * Connection address
             */
            std::string ipv4;

            /**
             * Connection port
             */
            int port;


        };

    }
}