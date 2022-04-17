#include <stdio.h> // printf
#include <stdlib.h> // exit()
#include <sys/types.h> // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close()
#include <string.h> // memset
#include <arpa/inet.h> // inet_addr()

typedef struct msg {
	short cnt; // 2byte
	short oper1; // 2byte
	short oper2; // 2byte
	short oper3; // 2byte
	char op; // 1byte
	char unuse; // 1byte
} MSG;

typedef struct answer {
	int result;
} ANSWER;

int main(void) 
{
    // client 
	int client_sock;
	struct sockaddr_in client;
	MSG message;
	char opMessage[100];
	int inputChar = 0;
	
	// server
	struct sockaddr_in server;
	char* server_IP = "192.168.75.128";
	char* server_PORT = "30000";
	ANSWER messageFromServer;
	
	// etc
	int result = 0;
	
	// 1. socket()
	// - IPPROTO_UDP: AF_INET과 SOCK_DGRAM을 모두 만족하는 프로토콜
	client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_sock == -1) {
		printf("socket error");
		exit(1);
	}
	printf("client_sock = %d \n", client_sock);
	
	// - 연결할 서버의 IP와 PORT 번호 초기화 및 할당
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(server_PORT));
	server.sin_addr.s_addr = inet_addr(server_IP);
	
	// 2. connect
	result = connect(client_sock, (struct sockaddr*) &server, sizeof(server));
	if(result == -1) {
		perror("connect error");
		exit(1);
	}
	printf("connect OK \n");
	
    // 반복해서 서비스를 요청하기 위한 Send-Recv 루프
	while(1) {
		message.cnt = 2;
		
        // Oper 1,2와 Operator를 입력
		printf("Oper 1: ");
		scanf("\n%hd", &message.oper1);
		
		printf("Oper 2: ");
		scanf("\n%hd", &message.oper2);
		
		printf("Operator (+,-,*,/): ");
		scanf("\n%c", &message.op);
		
		message.unuse = 0;
		
		// 3. send 
		result = send(client_sock, &message, sizeof(message), 0);
		if(result != sizeof(message)) 
		{
			perror("send error");
			exit(1);
		}
		printf("Send Success!\n");
		
		// 4. recv
		memset(&messageFromServer, 0, sizeof(messageFromServer));
		result = recv(client_sock, &messageFromServer, sizeof(messageFromServer), 0);
		if(result == -1) 
		{
			perror("recv error");
			exit(1);
		}
		printf("Operation Result = %d\n", messageFromServer.result);
		printf("-----------------\n");
	}
	
	return 0;
	 
}