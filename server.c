#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include"server_functions.h"
#define PORT 8080
#define START_MENU 0
#define SIGN_UP_OPTIONS 1
#define SIGN_IN_OPTIONS 2
#define USER_OPTIONS 3
#define ADMIN_OPTIONS 4
#define SIGN_UP 5
#define SIGN_IN 6
#define SIGN_UP_AS_USER 7
#define SIGN_UP_AS_JOINT 8
#define SIGN_UP_AS_ADMIN 9
#define SIGN_IN_AS_USER 10
#define SIGN_IN_AS_JOINT 11
#define SIGN_IN_AS_ADMIN 12
#define DEPOSIT 13
#define WITHDRAW 14
#define BALANCE 15
#define PASSWORD 16
#define DETAILS 17
#define EXIT 18
#define ADD_USER 19
#define DEL_USER 20
#define MOD_USER 21
#define GET_USER_DETAILS 22
#define INVALID -1
#define BUF_SIZE 1000

int main(int argc, char const *argv[])
{
	int sd, new_socket, valread;
	struct sockaddr_in serveradd;
	int opt = 1;
	int len = sizeof(serveradd);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY;
	serveradd.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(sd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(sd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("ready to listen!!\n");
	while (1)
	{
		if ((new_socket = accept(sd, (struct sockaddr *)&serveradd, (socklen_t *)&len)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, connection_handler, (void *)&new_socket) < 0)
		{
			perror("Unable to create thread");
			return 1;
		}
		puts("Handler is assigned to the socket....");
	}
	return 0;
}
