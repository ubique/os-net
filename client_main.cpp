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
    client cl;

    try {
        cl.connect_to(argv[1], std::atoi(argv[2]));
        string req;
        cout << "Type your messages" << std::endl;
        while (cin >> req) {
            cout << cl.request(req) << std::endl;
        }
    } catch (std::runtime_error &error) {
        std::cerr << error.what() << std::endl;
        try {
            cl.disconnect();
        } catch (std::runtime_error &close_er) {
            std::cerr << close_er.what() << std::endl;
        }
    }
    return 0;
}