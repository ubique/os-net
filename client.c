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
	int message_len;
	int recv_len;
	int buff_len;

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
		fprintf(stderr, "connect error\n");
		exit(1);
	}

	printf("enter a message:\n");
	fgets(message, MAX_BUFF_SIZE, stdin);
	message_len = strlen(message);

	if (send(sock, message, message_len, 0) < 0) {
		fprintf(stderr, "send error\n");
		exit(4);
	}

	bzero(buff, MAX_BUFF_SIZE);
	buff_len = 0;

	while (message_len > buff_len) {
		if ((recv_len = recv(sock, buff + buff_len, sizeof(buff) - buff_len, 0)) < 0) {
			fprintf(stderr, "recv error\n");
			exit(2);
		}
		buff_len += recv_len;
	}
	printf("server answered: %s\n", buff);

	close(sock);
}