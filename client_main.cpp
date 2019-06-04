//
// Created by roman on 20.05.19.
//

#include <iostream>
#include "client.h"
#include <stdio.h>

using std::cin;
using std::cout;
using std::string;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << argv[0] << " [server address] [port]" << std::endl;
        return 0;
    }
    client cl;
    cl.connect_to(argv[1], std::atoi(argv[2]));
    string req;
    cout << "Type your requests:" << std::endl;
    while (cin >> req) {
        cout << cl.request(req) << std::endl;
        if (req == "stop") {
            break;
        }
    }
    return 0;
}