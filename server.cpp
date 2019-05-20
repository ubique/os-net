#include<bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 2) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help")) {
            cout << "Try: server [one of the following]" << endl;
            cout << "\t any [PORT]" << endl;
            cout << "\t\t opens socket on port equals PORT" << endl;
            cout << "\t [IP] [PORT]" << endl;
            cout << "\t\t opens socket on ip equals IP and port equals PORT" << endl;
            cout << "While server is online you can try COMMAND as client:" << endl;
            cout << "\tCOMMAND = exit$. Closes server and client" << endl;
            return 0;
        }
        return -1;
    }
    if(argc != 3) {
        cerr << "Error: illegal amount of arguments" << endl;
        cout << "Try: server [-help | -h]" << endl;
        return -1;
    }

    const char* ip = argv[1];
    const char* port = argv[2];

    int sock, listener;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);

    if(listener == -1) {
        cerr << strerror (errno) << endl;
        exit(-1);
    }

    int yes = 1;
    if(setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) == -1) {
        cerr << strerror (errno) << endl;
        if(close(listener) == -1) {
            cerr << strerror (errno) << endl;
        }
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(stol(port));

    if(!strcmp(ip, "any")) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else {
        addr.sin_addr.s_addr = inet_addr(ip);
    }

    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        cerr << strerror (errno) << endl;
        if(close(listener) == -1) {
            cerr << strerror (errno) << endl;
        }
        exit(-1);
    }

    if(listen(listener, 1) == -1) {
        if(close(listener) == -1) {
            cerr << strerror (errno) << endl;
        }
        exit(-1);
    }

    while(1) {
        socklen_t size = sizeof(addr);
        sock = accept(listener, (struct sockaddr *)&addr, &size);

        if(sock < 0) {
            cerr << strerror (errno) << endl;
            if(close(listener) == -1) {
                cerr << strerror (errno) << endl;
            }
            exit(-1);
        }

        while(1) {
            char buf[1024];
            int bytes_read;
            string received_string = "";
            do {
                bytes_read = recv(sock, buf, 1024, 0);
                if(bytes_read <= 0) {
                    break;
                }
                received_string += string(buf).substr(0, bytes_read);
            } while(buf[bytes_read - 1] != -1);
            if(bytes_read <= 0) {
                break;
            }
            received_string = received_string.substr(0, received_string.size() - 1);
            if(received_string == "exit$") {
                send(sock, (const char*)NULL, 0, 0);
                if(close(sock) == -1) {
                    cerr << strerror (errno) << endl;
                }
                if(close(listener) == -1) {
                    cerr << strerror (errno) << endl;
                }
                return 0;
            }
            for(int i = 0; i < received_string.size(); i++) {
                cout << received_string[i];
            }
            cout << endl;
            cout.flush();
            received_string+=-1;
            int sent = 0;
            do {
                string message = received_string.substr(sent, received_string.size());
                int bytes_sent = send(sock, message.c_str(), message.size(), 0);
                if(bytes_sent <= 0) {
                    cerr << "Error while sending" << endl;
                    break;
                }
                sent += bytes_sent;
            } while (sent< received_string.size());
        }
        if(close(sock) == -1)
        {
            cerr << strerror (errno) << endl;
        }
    }
    if(close(listener) == -1) {
        cerr << strerror (errno) << endl;
    }
    return 0;
}
