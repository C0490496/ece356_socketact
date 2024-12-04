#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main(){
	int socket_fd; //Socket descriptor for the client.
	struct sockaddr_in server_addr; //Create a struct to hold the server address.
	char buffer[BUFFER_SIZE];	//Buffer for sending and receiving data.

	//Step 1: Create a socket.
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	//Step 2: Set the server address parameters.
	server_addr.sin_family = AF_INET;	//IPv4 address familt.
	server_addr.sin_port = htons(PORT);	//Port number in the network.
	if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0){
		perror("Invalid address or address not supported");	//Error if IP address conversion fails.
		exit(EXIT_FAILURE);
	}

	//Step 3: Connect to the server.
	if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}

	printf("Connected to the server .\n");

	//Loop to maintain communication with the server.
	while(1){
		printf("Enter city name: (enter 'exit' to stop) \n Available City Options - Victoria, Vancouver, Calgary, or Toronto.\n");
		fgets(buffer, BUFFER_SIZE, stdin);	//Read the user input for the city name.

		if(strncmp(buffer, "exit", 4) ==0){
			break; //Exit the loop if the input is 'exit'
		}

		send(socket_fd, buffer, strlen(buffer), 0); //Send the city name to the server.
		memset(buffer, 0, BUFFER_SIZE); 	//Clear the buffer for receiving data.

		int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);	//Receive server's response.
		if(bytes_received > 0){
			buffer[bytes_received] = '\0'; //Null-terminates the received string.
			printf("Weather Info: %s \n", buffer);	//Display the weather information of the corresponding city.
		}
	}

	//Last but not least Close the socket.
	close(socket_fd);
	printf("Disconnected from the server. \n"); //Notify the user of the disconnection.
	return 0;
}
