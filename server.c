#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>

#define MAX_BUFF_SIZE 1024

int main (int argc, char const*argv[]) {

	int port;
	int max_connection;
	int sock;
	struct sockaddr_in server_addr;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <ip_address> <port> <max_connection_num>\n", argv[0]);
		exit(0);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	printf("%d %d\n", server_addr.sin_port, server_addr.sin_addr.s_addr);
	if (bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "bind error %s\n", strerror(errno));
		close(sock);
		exit(1);
	}

	if (listen(sock, atoi(argv[3])) < 0) {
		fprintf(stderr, "listen error\n");
		exit(2);
	}

	int client_sock;
	char message[MAX_BUFF_SIZE];
	int message_len;
	int send_len;

	printf("Server is running...\n");

	while (1) {
		bzero(message, MAX_BUFF_SIZE);

		if ((client_sock = accept(sock, (struct sockaddr*) NULL, NULL)) < 0) {
			fprintf(stderr, "accept error\n");
		}

		if ((message_len = recv(client_sock, &message, sizeof(message), 0)) < 0) {
			fprintf(stderr, "recv error\n");
		}

		while (message_len > 0) {

			if (strcmp(message, "exit\n") == 0) {
				close(client_sock);
				printf("Server stopped\n");
				close(sock);
				return 0;
			}
			if ((send_len = send(client_sock, message, sizeof(message), 0)) < 0) {
				fprintf(stderr, "send error");
			}
			message_len -= send_len;
		}

		close(client_sock);
	}
}