#include<bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

bool is_equal(const char* a, const char* b,const int& len) {
    for(int i = 0; i < len; i++) {
        if(a[i]!=b[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[]) {
    if(argc == 2) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help")) {
            cout << "Try: client [one of the following]" << endl;
            cout << "\tlocalhost [PORT]" << endl;
            cout << "\t\topens socket on port equals PORT" << endl;
            cout << "\t[IP] [PORT]" << endl;
            cout << "\t\topens socket on ip equals IP and port equals PORT" << endl;
            return 0;
        }
        return -1;
    }
    if(argc != 3) {
        cerr << "Error: illegal amount of arguments" << endl;
        cout << "Try: client [-help | -h]" << endl;
        return -1;
    }
    const char* ip = argv[1];
    const char* port = argv[2];
    string s;
    while(getline(cin, s)) {
        s += -1;
        int sock;
        struct sockaddr_in addr;

        sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock < 0) {
            cerr << strerror (errno) <<endl;
            exit(-2);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(stol(port));

        if(!strcmp(ip,"localhost")) {
            addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        }
        else {
            addr.sin_addr.s_addr = inet_addr(ip);
        }
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            cerr << strerror (errno) <<endl;
            if(close(sock) == -1)
            {
                cerr << strerror (errno) <<endl;
            }
            exit(-3);
        }

        string recived_message = "";
        string message;

        int sent = 0;
        do {
            message = s.substr(sent, s.size());
            int send_bytes = send(sock, message.c_str(), message.size(), 0);
            if(send_bytes <= 0) {
                cerr << strerror (errno) <<endl;
                if(close(sock) == -1) {
                    cerr << strerror (errno) <<endl;
                }
                exit(-1);
            }
            sent += send_bytes;
        } while(sent < s.size());
        char buf[1024];
        int recvd_bytes = 0;
        do {
            recvd_bytes = recv(sock, buf, 1024, 0);
            if(recvd_bytes <= 0) {
                if(message.substr(0, message.size()-1) != "exit$") {
                    cerr << strerror (errno) <<endl;
                }
                if(close(sock) == -1) {
                    cerr << strerror (errno) <<endl;
                }
                exit(-1);
            }
            recived_message += string(buf).substr(0, recvd_bytes);
        } while(buf[recvd_bytes - 1] != -1);
        recived_message = recived_message.substr(0, recived_message.size() - 1);
        if (recived_message == "exit$") {
            if(close(sock) == -1) {
                cerr << strerror (errno) <<endl;
            }
            return 0;
        }
        if(close(sock) == -1) {
            cerr << strerror (errno) <<endl;
        }
    }
    return 0;
}
