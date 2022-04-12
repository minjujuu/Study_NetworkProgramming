// TCP/IPv4 Server Program
#include <stdio.h> // printf
#include <stdlib.h> // exit()
#include <sys/types.h> // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close()
#include <string.h> // memset
#include <arpa/inet.h> // inet_addr()

int main(int argc, char *argv[]) 
{
	int ser_sock;
	int accept_no = 0;
	struct sockaddr_in server;
	struct sockaddr_in client;
	char temp[100];
	
	int n = 0, client_length = 0;
	char* ser_ip = "";
	char* ser_port = "30000";
	
	// 1. socket()
	ser_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(ser_sock == -1) {
		printf("socket error \n");
		exit(1);
	}
	
	printf("server socket = %d \n", ser_sock);

	memset(&server, 0, sizeof(server)); // 16byte 0 clear
	server.sin_family      = AF_INET; // IPv4: 2byte
	server.sin_port        = htons(atoi(ser_port)); // port number: 2byte
	server.sin_addr.s_addr = inet_addr(ser_ip); // 4byte

    // 2. bind()
	n = bind(ser_sock, (struct sockaddr *) &server, sizeof(server));
	if(n == -1) {
		perror("bind error");
		exit(1);
	} 
	printf("server bind success!\n");
	
    // 3. listen()
	if(listen(ser_sock, 100) == -1) {
		perror("listen error");
		exit(1);
	}
	printf("server listen success!\n");
	
	client_length = sizeof(client);
    // 4. accept()
	accept_no = accept(ser_sock, (struct sockaddr*) &client, &client_length);
	if(accept_no == -1) {
		perror("accept error");
		exit(1);
	}
	printf("server accept success! accept_no = %d \n", accept_no);
	
	memset(temp, 0, sizeof(temp)); // init
	
    // 5. receive()
	// 첫 번째 인자로는 server의 socket이 아닌 accept의 socket number를 적어주어야 함!
	n = recv(accept_no, temp, sizeof(temp), 0);
	if(n == -1) {
		perror("server recv error");
	}
    
	printf("Server Recv : %s\n", temp);
	return 0;
	
}