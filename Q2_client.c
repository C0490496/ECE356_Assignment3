#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024


//Broadcase a message to all clients excluding the sender.
void *receive(void *arg){
	int client_socket = *((int *)arg);
	char buffer[BUFFER_SIZE];

	while(1){
		memset(buffer, 0, BUFFER_SIZE);
		int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

		if(bytes_received <= 0){
			printf("Connection to server lost. \n");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		printf("%s", buffer);
	}
	return NULL;
}

int main(){
	int client_socket;
	struct sockaddr_in server_address;
	char buffer[BUFFER_SIZE];

	//Step 1: Create a socket
	if((client_socket = socket(AF_INET, SOCK_STREAM,0))<0){
		perror("Socket creation error");
		exit(EXIT_FAILURE);
	}
	printf("Socket created successfully. \n");


	//Step 2: Connect to the server.
	server_address.sin_family=AF_INET;
	server_address.sin_addr.s_addr=INADDR_ANY;
	server_address.sin_port=htons(PORT);

	if(connect(client_socket,(struct sockaddr *)&server_address, sizeof(server_address)) < 0 ){
		perror("Connection failed");
		close(client_socket);
		exit(EXIT_FAILURE);
		}
	printf("Connection was successful");

	//Step 3: Create a thread to receive messages.
	pthread_t thread;
	if(pthread_create(&thread, NULL, receive, (void *)&client_socket) != 0){
		perror("Thread creation failed");
		close(client_socket);
		exit(EXIT_FAILURE);
	}

	while(1){
		memset(buffer, 0 , BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);

		if(strncmp(buffer, "exit", 4) == 0){
			printf("Disconnecting...\n");
			close(client_socket);
			exit(EXIT_SUCCESS);
		}
		if(send(client_socket, buffer, strlen(buffer), 0)<0){
			perror("Message has failed sending");
		}
	}
return 0;

}

