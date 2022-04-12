// TCP/IPv4 Server Program
#include <stdio.h> // printf
#include <stdlib.h> // exit()
#include <sys/types.h> // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close()
#include <string.h> // memset
#include <arpa/inet.h> // inet_addr()

typedef struct msg {
	short cnt;
	short oper1;
	short oper2; // 2byte
	short oper3; // 2byte
	char op; // 1byte
	char unuse; // 1byte
} MSG;

typedef struct ans {
	int sum;
} ANSWER;

int main() 
{
	int ser_sock, accept_no;
	struct sockaddr_in server, client;

	char temp[100];
	
	int n = 0, client_length = 0;
	char* ser_ip = "192.168.75.128";
	char* ser_port = "30000";
	
	MSG cli_msg;
	ANSWER cli_result;
	
	// 1. socket 
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

	// 2. bind
	n = bind(ser_sock, (struct sockaddr *) &server, sizeof(server));
	if(n == -1) {
		perror("bind error");
		exit(1);
	} 
	printf("server bind success!\n");
	// 3. listen
	if(listen(ser_sock, 100) == -1) {
		perror("listen error");
		exit(1);
	}
	
	while(1) {
	printf("server listen success!\n");
	
	client_length = sizeof(client);
	// 4. accept
	accept_no = accept(ser_sock, (struct sockaddr*) &client, &client_length);
		if(accept_no == -1) {
		perror("accept error");
		exit(1);
	}
	printf("server accept success! accept_no = %d \n", accept_no);
	
	memset(&cli_msg, 0, sizeof(cli_msg));
	// 5. recv
	n = recv(accept_no, &cli_msg, sizeof(cli_msg), 0);
	if(n == -1) {
		perror("server recv error");
	}
	printf("cli_msg.cnt = %d \n", cli_msg.cnt);
	printf("cli_msg.oper1 = %d \n", cli_msg.oper1);
	printf("cli_msg.oper2 = %d \n", cli_msg.oper2);
	printf("cli_msg.op = %c \n", cli_msg.op);
	
	
	switch(cli_msg.op) {
		case '+':
			cli_result.sum = cli_msg.oper1 + cli_msg.oper2;
			break;
		case '-':
			cli_result.sum = cli_msg.oper1 - cli_msg.oper2;
			break;
		case '*':
			cli_result.sum = cli_msg.oper1 * cli_msg.oper2;
			break;
		case '/':
			cli_result.sum = cli_msg.oper1 / cli_msg.oper2;
			break;			
		default:
			cli_result.sum = 0;
			break;
	}
	// send 2nd parameter 
	// case structure, use address symbol '&'
	n = send(accept_no, &cli_result, sizeof(cli_result), 0);
	if(n != sizeof(cli_result)) {
		perror("send error");
		exit(1);
	}
	printf("Server Send: %d \n", cli_result.sum);
	}
	printf("\n\n Konkuk Univ. 201914302 Minju Park");
	return 0;	
}