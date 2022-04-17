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

	int client_socket = 0;
	int str_len = 0;
	struct sockaddr_in server;
	char* ip = "127.0.0.1";
	char* port = "30000";
	
	char buffer[BUFSIZE];
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons(atoi(port));
	
	connect(client_socket, (struct sockaddr*) &server, sizeof(server));
	
	while(1) {
		printf("Input Message: ");
		
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, BUFSIZE, stdin);
		send(client_socket, buffer, BUFSIZE, 0);
		
		memset(buffer, 0, sizeof(buffer));
		str_len = recv(client_socket, buffer, BUFSIZE-1, 0);
		buffer[str_len]=0;
		
		printf("Message from server: %s\n", buffer);
	}
	
	close(client_socket);
	

	return 0;
}