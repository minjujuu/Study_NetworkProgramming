#include <stdio.h> // printf 
#include <stdlib.h> // exit
#include <sys/types.h> // socket 
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close()
#include <string.h> // memset
#include <arpa/inet.h> // inet_addr()

#define BACKLOG 100

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

// server process
// 1. socket()
// 2. bind()
// 3. listen()
// 4. accept()
// 5. send()/recv()
// 6. close()

int main(void)
{
	// server
	int server_sock = 0;
	int pid = 0; // 멀티프로세스 기반의 다중접속 서버 구현을 위한 변수 
	int accept_no = 0;
	struct sockaddr_in server;
    // 서버 정보 
	char* ser_ip = "192.168.75.128";
	char* ser_port = "30000";
	ANSWER oper_answer;
	
	// client
	struct sockaddr_in client;
	int client_adr_size = 0;
	MSG client_message;
	
	// etc
	int result = 0;
	
	// 1. create socket
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_sock == -1) 
	{
		printf("socket error");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(ser_port));
	server.sin_addr.s_addr = inet_addr(ser_ip);
	
	// 2. bind
	result = bind(server_sock, (struct sockaddr*) &server, sizeof(server));
	if(result == -1) 
	{
		perror("bind error");
		exit(1);	
	}
	printf("server bind success!\n");
	
	// 3. listen
	result = listen(server_sock, BACKLOG);
	if(result == -1)
	{
		perror("listen error");
	}
	printf("server listen success!\n");
	
	while(1) // 다중 접속을 받기 위한 루프
	{
		client_adr_size = sizeof(client);
        // accept
        // - 연결요청 대기 큐에서 첫번째로 대기 중에 있는 연결 요청을 참조하여 클라이언트와의 연결을 구성
        // - 이 때 생성된 소켓의 파일 디스크립터를 반환
        // - 이 함수가 호출되었을 때 대기큐가 비어있는 상태라면, 대기 큐가 찰 때까지, 
        //   즉 클라이언트의 연결 요청이 들어올 때까지 accept 함수는 반환하지 않는다. 
        // - 따라서 while loop에서 해당 함수를 호출하면 다음 클라이언트의 요청이 들어올 때까지 서버가 대기할 수 있게 된다.
		accept_no = accept(server_sock, (struct sockaddr*) &client, &client_adr_size);
		if(accept_no == -1) {
			continue; // go loop
		} else {
			puts("New client connected...");
		}

		// create process
		pid = fork();
		if(pid == -1) {
			close(accept_no);
			continue; // go loop
		}
		
        // child process
		if(pid == 0) { 
			close(server_sock); // 자신과 상관없는 소켓의 파일 디스크립터 닫아줌
			memset(&client_message, 0, sizeof(client_message));
			memset(&oper_answer, 0, sizeof(oper_answer));
			
            // 계속해서 서비스를 제공하기 위한 recv-send 루프
            // * 서버에서는 recv, send 시 accept로부터 반환된 소켓을 사용함 

			// recv
            // - 마지막 인자는 옵션 값인데 0으로 주면 됨 
			while((result = recv(accept_no, &client_message, sizeof(client_message), 0)) != 0) {
                // service 제공
				if(result == -1) {
					perror("recv error");
				}
				printf("client_message.cnt = %d \n", client_message.cnt);
				printf("client_message.oper1 = %d \n", client_message.oper1);
				printf("client_message.oper2 = %d \n", client_message.oper2);
				printf("client_message.op = %c \n", client_message.op);

				switch(client_message.op) {
				case '+':
					oper_answer.result = client_message.oper1 + client_message.oper2;
					break;
				case '-':
					oper_answer.result = client_message.oper1 - client_message.oper2;
					break;
				case '*':
					oper_answer.result = client_message.oper1 * client_message.oper2;
					break;
				case '/':
					oper_answer.result = client_message.oper1 / client_message.oper2;
					break;			
				default:
					oper_answer.result = 0;
					break;
				}
				
				// send 
                // - 마지막 인자는 옵션 값인데 0으로 주면 됨 
				result = send(accept_no, &oper_answer, sizeof(oper_answer), 0);
				if(result != sizeof(oper_answer)) {
					perror("send error");
					exit(1);
				}
				printf("Operation Result = %d\n", oper_answer.result);
				printf("-----------------\n");
			}

			close(accept_no); // 자신이 연결했던 클라이언트의 소켓 닫아줌 
			puts("client disconnected ...");
		} else {
            // 부모는 자식에게 넘겨준 소켓의 파일 디스크립터를 닫아줌 (소켓에 연결된 파일 디스크립터가 둘 이상 존재하면 닫히지 않으므로)
			close(accept_no);
		}	
	}
	close(server_sock);
	return 0;
}

