#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

	printf("socket creation success..\n");

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(sockfd, (struct sockaddr *) &servaddr,
			   sizeof(servaddr))) != 0) {
		printf("Socket bind failed...\n");
		exit(-1);
	}

	printf("Socket binding success...\n");

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(-1);
	}

	printf("server listening...\n");
	len = sizeof(cli);

	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);

	if (connfd < 0) {
		printf("Server accept failed...\n");
		exit(-1);
	}

	printf("Server accept the client...\n");

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
		if (!strncmp(buf, "PING", 4)) {
			const char* pong = "PONG\n";
			int n = send(connfd, pong, strlen(pong), 0);
			if (n < 0) {
				printf("Error writing to socket.\n");
				exit(-1);
			}
		} else {
			printf("Unexpected message received from client.\n");
			exit(-1);
		}
	}

	return 0;
}