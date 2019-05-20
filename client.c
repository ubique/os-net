#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFF_SIZE 1024


int main (int argc, char const *argv[]) {
	int port;
	int sock;
	struct sockaddr_in addr;
	char message[MAX_BUFF_SIZE];
	char buff[MAX_BUFF_SIZE];

	if (argc != 3) {
		fprintf(stderr, "usage: %s <ip_address> <port>\n", argv[0]);
		exit(0);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "connect erro r\n");
		exit(1);
	}

	printf("enter a message:\n");
	fgets(message, MAX_BUFF_SIZE, stdin);
	send(sock, message, strlen(message), 0);

	bzero(buff, MAX_BUFF_SIZE);

	if ((recv(sock, &buff, sizeof(buff), 0)) < 0) {
		fprintf(stderr, "recv\n");
		exit(2);
	}

	if (strncmp(buff, "exit", strlen(buff)) != 0) {
		printf("server answered: %s\n", buff);
	}

	close(sock);
}