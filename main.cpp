#include <iostream>
#include <vector>
#include <string.h>
#include <map>
#include "utils/Utils.h"
#include "server/TCPServer.h"
#include "client/TCPClient.h"
#include "protocol/ECHOProtocol.h"

int main(int argc, char** argv) {
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        Utils::help();
        return 0;
    }
    if (argc < 2) {
        Utils::error("Wrong arguments count. Try '--help'");
        return 0;
    }
    bool isServer;
    if (strcmp(argv[1], "server") == 0) {
        isServer = true;
    } else if (strcmp(argv[1], "client") == 0) {
        isServer = false;
    } else {
        Utils::error("Undefined argument '" + std::string(argv[1]));
        return 0;
    }
    auto args = Utils::argParse(!isServer, argc, argv);
    if (args.empty()) {
        return 0;
    }
    net::Protocol* protocol = new net::ECHOProtocol();


    auto ip = args["a"];
    auto port = args["p"];


    data_t data;

    if (isServer) {
        net::TCPServer server(ip, port, protocol);
        if (!server.create()) {
            return 0;
        }
        server.run(data);
    } else {
        for (auto c: args["d"]) {
            data.push_back(c);
        }

        Utils::message("Input data: " + std::to_string(data.size()));
        net::TCPClient client(ip, port, protocol);
        if (!client.create()) {
            return 0;
        }
        client.run(data);
    }

    return 0;
}