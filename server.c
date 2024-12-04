#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

//Struct to store city weather information.
typedef struct{
	char city[50];			//Name of the City.
	char weather[BUFFER_SIZE];	//Weather details for the specific city.
}CityWeather;

//Predefined list of cities and their correspoding weather status.
CityWeather cities[] = {
	{"Victoria", "\nTemperature: 11C \nHumidity: 60% \nClear sky"},
	{"Vancouver", "\nTemperature: 15C \nHumidity: 76% \nCloudy sky"},
	{"Calgary", "\nTemperature: 5C \nHumidity : 30% \nClear sky"},
	{"Toronto", "\nTemperature: 7C \nHumidity: 40%\nRainy"}
};

int city_count = sizeof(cities)/ sizeof(cities[0]); //Given count of  the predefined cities.

//Function to handle the client requests
void *handle_client(void *arg){
	int client_socket = *(int * )arg;	//Retrieve the client socket descriptor.
	free(arg); 				//Free the allocated memory for the socket descriptor.
	char buffer[BUFFER_SIZE];		//Buffer for the client messages.

	while(1){
		memset(buffer, 0, BUFFER_SIZE);	//Clear the buffer.
		int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);	//Receive the data from the client.
		if(bytes_received <= 0){	//If no data or error then break out of the while loop.
			break;
		}
		buffer[strcspn(buffer, "\n")] = '\0'; 	//Remove the newline character from the input.
		printf("Client requested weather for: %s\n", buffer);

		//Search for the city in the predefined list.
		char response[BUFFER_SIZE] = "City does not exist or cannot be found.";
		for(int i = 0; i < city_count; i++){
			if(strcasecmp(cities[i].city, buffer) == 0){	//This allows the comparison to not be case sensitive.
				strncpy(response, cities[i].weather, BUFFER_SIZE);	//Copies the weather details to the response.
				break;
			}
		}
		send(client_socket, response, strlen(response), 0); //This sends a response back to the client.
	}
	close(client_socket);//Close the client socket.
	printf("Client Disconnected. \n");
	return NULL;
}

int main(){
	int server_socket, client_socket, *new_sock; //Socket descriptors.
	struct sockaddr_in server_addr, client_addr; //Server and cient address structures.
	socklen_t client_len = sizeof(client_addr);  //Length of client address structure.

	//Step 1:Create a socket.
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("Socket creation failed"); //Print and error if the socket creations has failed.
		exit(EXIT_FAILURE);
	}

	//Step2: Configure the server address.
	server_addr.sin_family = AF_INET;	//IPv4.
	server_addr.sin_addr.s_addr= INADDR_ANY; //Accept connections from any IP.
	server_addr.sin_port = htons(PORT); 	//Set the port number.

	//Step 3: Bind the socket to the specified address and port.
	if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("Bind Failed");
		exit(EXIT_FAILURE);
	}

	//Step 4: Listen for any incoming connections.
	if(listen(server_socket, 5) < 0){ //Allow only up to 5 connections.
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}
	printf("Server is running on port: (%d) \n", PORT);

	//Loop to accept and handle client connections.
	while((client_socket = accept( server_socket, (struct sockaddr *)&client_addr, &client_len))){
		printf("Client has connected.\n");

		//Allocate memory for the new client.
		new_sock = malloc(sizeof(int));
		*new_sock = client_socket;

		//Create a new thread for the new client.
		pthread_t thread;
		if(pthread_create(&thread, NULL, handle_client, (void *)new_sock) < 0){
			perror("Thread creation failed");
			free(new_sock);				//Free up allocated memory if the thread creation has failed.
			continue;
		}
		pthread_detach(thread);	//Detach the thread to handle clients independently.
	}

	//If the accept does not work print and error and exit gracefully.
	if(client_socket<0){
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}

	close(server_socket); //Close the server socket.
	return 0;
}
