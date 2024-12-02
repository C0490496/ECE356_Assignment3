#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct{
	int socket;
	struct sockaddr_in address;
}Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

//Broadcase a message to all clients excluding the sender.
void broadcast(const char *message, int sender_socket){
	pthread_mutex_lock(&clients_mutex);
	for(int i = 0; i <client_count; i++){
		if(clients[i].socket != sender_socket){
			if(send(clients[i].socket, message, strlen(message), 0) < 0){
				perror("Error boradcasting message");
				close(clients[i].socket);
				clients[i] = clients[--client_count]; //Remove the client
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg){
	Client client = *((Client *)arg);
	char buffer[BUFFER_SIZE];
	char welcome_msg[BUFFER_SIZE];

	sprintf(welcome_msg, "Client %d: Connected. \n", client.socket);
	printf("%s", welcome_msg);
	broadcast(welcome_msg, client.socket);

	while(1){
		memset(buffer, 0 , BUFFER_SIZE);
		int bytes_received = recv(client.socket, buffer, BUFFER_SIZE, 0);


		if(bytes_received <= 0){
			printf("Client %d: Disconnected. \n", client.socket);
			close(client.socket);

			pthread_mutex_lock(&clients_mutex);
			for(int i = 0; i<client_count; i++){
				if(clients[i].socket == client.socket){
					clients[i] = clients[--client_count];
					break;
				}
			}
			pthread_mutex_unlock(&clients_mutex);

			break;
		}
		printf("Client %d: %s", client.socket, buffer);
		broadcast(buffer, client.socket);
	}
	return NULL;
}



int main(){
int server_socket;
struct sockaddr_in server_address;

//Step 1: Create a socket
if((server_socket = socket(AF_INET, SOCK_STREAM,0))==-1){
	perror("Socket creation error");
	exit(EXIT_FAILURE);
}
printf("Socket created successfully. \n");


//Step 2: Bind the socket to an IP and port.
server_address.sin_family=AF_INET;
server_address.sin_addr.s_addr=INADDR_ANY;
server_address.sin_port=htons(PORT);

if(bind(server_socket,(struct sockaddr *)&server_address, sizeof(server_address)) < 0 ){
perror("Bind failed");
close(server_socket);
exit(EXIT_FAILURE);
}
printf("Bind was successful\n");

//Step 3: Listen for incoming connections. 
if(listen(server_socket, MAX_CLIENTS)<0){
perror("Listening Failed");
close(server_socket);
exit(EXIT_FAILURE);
}
printf("Server is listening on port %d... \n", PORT);

//Step 4: Accept the client connection.
while(1){
	Client client;
	socklen_t client_len = sizeof(client.address);
	client.socket = accept(server_socket, (struct sockaddr *)&client.address, &client_len);

	if(client.socket<0){
		perror("Accept failed");
		continue;
	}
	printf("Accept successful\n");

	pthread_mutex_lock(&clients_mutex);
	clients[client_count++] = client;
	pthread_mutex_unlock(&clients_mutex);

	pthread_t thread;
	if(pthread_create(&thread, NULL, handle_client, (void *)&client) != 0){
		perror("Thread creation failed");
	}
	pthread_detach(thread);
}

close(server_socket);
return 0;

}

