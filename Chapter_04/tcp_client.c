// TCP/IPv4 Client Program
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
	int client_sock;
	struct sockaddr_in server;

	int n = 0;
	
	char* server_IP = "";
	char* server_PORT = "30000";
	
	char data[100];
	
	// 1. socket() --------------------------------------
	// - IPPROTO_UDP: AF_INET과 SOCK_DGRAM을 모두 만족하는 프로토콜
	client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(client_sock == -1) {
		printf("socket error \n");
		exit(1);
	}

	printf("client_sock = %d \n", client_sock);
	
    // 연결할 서버의 IP와 포트번호 
	memset(&server, 0, sizeof(server)); // 16byte 0 clear
	server.sin_family  	= AF_INET; // IPv4: 2byte
	server.sin_port 	= htons(atoi(server_PORT)); // port number: 2byte
	server.sin_addr.s_addr = inet_addr(server_IP); // 4byte

    // 2. connect() --------------------------------------
    // - NOTE: 서버의 listen이 호출되어 있어야 클라이언트의 connect가 유효
	if(connect(client_sock, (struct sockaddr*) &server, sizeof(server)) == -1) 		{
		perror("connect error");
		exit(1);
	}
	printf("connect OK \n");

	stpcpy(data, "Hello Server~~~");
	
    // 3. send() --------------------------------------
	n = send(client_sock, data, strlen(data), 0);
	if(n != strlen(data)) {
		perror("send error");
		exit(1);
	}
	 
	printf("send success! : strlen(data) = %d\n", n);
	
	return 0;
	
}