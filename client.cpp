#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

using std::cin, std::cout, std::endl, std::cerr;

void print_usage_args_and_exit() {
  cerr << "Wrong arguments: \"<host>:<port> <message>\" expected, port from 1 to 65535" << endl;
  cerr << "Example: \"127.0.0.1:8080\"" << endl;
  cerr << "NB: adress 255.255.255.255 isn't valid" << endl;
  exit(0);
}

void trying_again_and_log_cerr(std::string err, int try_cnt) {
  cerr << err << " failure. " << std::strerror(errno) << endl;
  if (try_cnt == 2) {
    exit(0);
  }
  cerr << "trying again" << endl;
  sleep(3);
}

int main(int argc, char** argv)
{
  //Args checking
  if (argc != 3) {
    print_usage_args_and_exit();
  }

  std::string a = argv[1],
              host = "",
              port = "";
  std::size_t found = a.find(":");

  if (found == std::string::npos) {
    print_usage_args_and_exit();
  }

  host = a.substr(0, found);
  port = a.substr(0, found + 1);
  if (inet_addr(host.c_str()) == INADDR_NONE || port == "") {
    print_usage_args_and_exit();
  }
  try {
    if (stoi(port) < 1 || stoi(port) > 65535) {
      print_usage_args_and_exit();
    }
  } catch (...) {
    print_usage_args_and_exit();
  }
  
  //Initialisation
  int sockd;
  for (int i = 0; i < 3; i++) {
    sockd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockd != -1) {
      break;
    }
    trying_again_and_log_cerr("Socket initialisation", i);
  }

  sockaddr_in addr = {.sin_family = AF_INET, .sin_addr = {.s_addr = inet_addr(host.c_str())}, .sin_port = htons(stoi(port))};

  for (int i = 0; i < 3; i++) {
    if (connect(sockd, (sockaddr*) &addr, sizeof(addr)) != -1) {
      break;
    }
    trying_again_and_log_cerr("Conecting", i);
  }

  //Start
  size_t sent = 0;
  while (sent != strlen(argv[2])) {
    int num;
    for (int i = 0; i < 3; i++) {
      num = send(sockd, argv[2] +  sent, strlen(argv[2]) - sent, 0);
      if (num != -1) {
        break;
      }
      trying_again_and_log_cerr("Local sending", i);
    }

    if (!num) {
      break;
    }
    sent += num;
  }

  char buffer[1024];

  size_t received = 0;
  cout << "Answer from the server: ";
  while (received != strlen(argv[2])) {
    bzero(buffer, sizeof(buffer));
    int n = recv(sockd, &buffer, sizeof(buffer), 0);

    if (n == -1) {
      cerr << "Message delivery failure. " << std::strerror(errno) << endl;
      exit(0);
    }

    if (n == 0) {
      break;
    }
    received += n;

    for (size_t i = 0; i < n; i++) {
      cout << buffer[i];
    }
  }
  cout << endl;

  for (int i = 0; i < 3; i++) {
    if (close(sockd) != -1) {
      break;
    }
    trying_again_and_log_cerr("Closeing", i);
  }

  return 0;
}