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

// client process
// 1. socket()
// 2. bind()
// 5. send()/recv()
// 6. close()

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
	// in_addr_t inet_addr(); 
    // - 문자열로 표현된 IP주소를 32비트 정수형으로 변환해주고, 변환과정에서 네트워크 바이트 순서(빅 엔디안)으로 변환해줌   
	
	// 2. connect
    // - 서버에 의해 연결요청이 접수되었거나, 네트워크 단절 등 오류상황일 경우에만 반환됨
    // - 단, 연결요청이 접수되었다는 것은 서버에서 accept가 호출된 게 아니라 연결요청 대기 큐에 등록된 것을 의미한다.
    // - 따라서 connect가 반환되었더라도 바로 서비스가 이루어지지 않을 수 있다. 
	result = connect(client_sock, (struct sockaddr*) &server, sizeof(server));
	if(result == -1) {
		perror("connect error");
		exit(1);
	}
    // 서버와 달리 클라이언트에서는 bind()를 명시적으로 호출할 필요가 없다.
    // - connect 호출 시 IP는 현재 호스트(컴퓨터)의 IP로, PORT는 임의로 선택해서 할당되기 때문이다.
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
		
        // * 클라이언트에서는 send, recv 시 클라이언트 소켓 사용함 (서버 소켓은 만든 적도 없음)
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