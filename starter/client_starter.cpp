//
// Created by Михаил Терентьев on 2019-05-13.
//
#include <iostream>
#include "../client-lib/client.h"
#include "../client-lib/client_exception.h"

void writeRequestMes();

using namespace std;

void printInfo() {
    cout << "echo-client: ";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Invalid args\n expected 2 args: [address] [port]\n";
        return -1;
    }
    try {
        client client(argv[1], static_cast<uint16_t>(stoul(argv[2])));
        printInfo();
        string text;
        while (getline(cin, text) && !text.empty()) {
            cout << "response message: " << client.request(text) << endl;
            printInfo();
        }
    } catch (client_exception &e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
