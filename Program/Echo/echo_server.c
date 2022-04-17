#include <stdio.h> // printf 
#include <stdlib.h> // exit
#include <sys/types.h> // socket 
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close()
#include <string.h> // memset
#include <arpa/inet.h> // inet_addr()
#define BUFSIZE 100
int main(void) {

	int server_socket = 0;
	struct sockaddr_in server, client;
	char* ip = "127.0.0.1";
	char* port = "30000";

	int client_socket = 0;
	int client_addr_size = 0;
	int str_len = 0;
	char buffer[BUFSIZE];
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons(atoi(port));
	
	// 1. socket
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// 2. bind
	bind(server_socket, (struct sockaddr*)&server, sizeof(server));
	
	// 3. listen
	listen(server_socket, 5);
	client_addr_size = sizeof(client);
	for(int i=0; i<5; i++) {
		
		
		// 4. accept
		client_socket = accept(server_socket, (struct sockaddr*) &client, &client_addr_size);
		
		// 5. recv
		memset(buffer, 0, sizeof(buffer));
		while((str_len = recv(client_socket, buffer, BUFSIZE, 0)) != 0) {
			strcat(buffer, "::SERVER");
			// 6. send
			send(client_socket, buffer, strlen(buffer), 0);
		}
		close(client_socket);
	}
	close(server_socket);

	return 0;
}