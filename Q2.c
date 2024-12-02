#include <stdlib.h>
#include <studio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int main(){
int server_fd, new_socket;
struct sockaddr_in server addr, client_addr;

socklen_t addr_len = sizeof(client_addr);
char buffer[BUFFER_SIZE] = {0};
char *response = "Hello from Server";
}

//Step 1: Create a socket 

if((server_fd = socket(AF_IET, SOCK_STREM,0))==0){
perror("Socket creation error");
exit(EXIT_FAILURE);
}
printf("Socket created successfully. \n");

//Step 2: Bind the socket to an IP and port.
server_addr.sin_family=AF_INET;
server_addr.sin_addr.s_addr=INADDR_ANY;
server_addr.sin_port=htons(PORT);

if(bind(server_fd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ){
perror("Bind failed");
close(server_fd);
exit(EXIT_FAILURE);
}
print("Bind was successful");

//Step 3: Listen for incoming connections. But we did realized that we wanted to listen to multiple connections using threads.
if(listen(server_fd, 3)<0){
perror("Listen Failed");
close(server_fd);
exit(EXIT_FAILURE);
}
printf("Server is listening on port %d... \n", PORT);

//Step 4: Accept the client connection.

//Step 5: Receive the data from the client.

//Step 6: Send the response to the client.

//Step 7: Close the sockets.
}

