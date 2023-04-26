#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

# define PORT 8148
# define MSG_BUF 1024

int main(int argc, char **argv)
{
	int sockfd;
	int connfd;
	int len;
	struct sockaddr_in servaddr;
	struct sockaddr_in cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed...\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		printf("setsockopt failed...\n");
		exit(-1);
	}

	if ((bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))) {
		printf("Socket bind failed...\n");
		exit(-1);
	}

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(-1);
	}

	len = sizeof(cli);

	while (true) {
		connfd = accept(sockfd, (struct sockaddr *)&cli, &len);

		if (connfd < 0) {
			printf("Server accept failed...\n");
			exit(-1);
		}

		char buf[MSG_BUF + 1];
		memset(buf, 0, MSG_BUF + 1);

		int n = recv(connfd, buf, sizeof(buf), 0);
		if (n < 0) {
			printf("Error reading from socket...\n");
			exit(-1);
		} else if (n == 0) {
			printf("Connection closed by client.\n");
			exit(0);
		} else {
			buf[n] = '\0';
			printf("Received message from client: %s\n", buf);
			char *msg;
			if (!strncmp(buf, "PING", 4)) {
				msg = "PONG\n";
				if (send(connfd, msg, strlen(msg), 0) < 0) {
					printf("Error writing to socket.\n");
					exit(-1);
				}
			} else {
				msg = "INVALID\n";
				if (send(connfd, msg, strlen(msg), 0) < 0)
					printf("Error writing to socket.\n");
				exit(-1);
			}
		}

		close(connfd);
	}

	close(sockfd);

	return 0;
}
