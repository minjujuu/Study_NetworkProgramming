#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	char message[] = "Hello World!";
	
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	/* TCP 소켓 생성 socket()
       @param domain(Protocol family) - PF_INET(IPv4 인터넷 프로토콜 체계)
       @param type(Socket type) - 소켓의 데이터 전송 방식 두 가지: 연결지향형 소켓(SOCK_STREAM), 비연결지향형 소켓(SOCK_DGRAM) 존재
       @param protocol - 하나의 프로토콜 체계 안에 데이터의 전송방식이 동일한 프로토콜이 둘 이상 존재하는 경우. 
                       - IPPROTO_TCP: PF_INET과 SOCK_STREAM을 모두 만족하는 프로토콜
                       - IPPROTO_UDP: PF_INET과 SOCK_DGRAM을 모두 만족하는 프로토콜
    */
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	if(serv_sock == -1)
		error_handling("socket() error");
	
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP주소
	serv_addr.sin_port = htons(atoi(argv[1])); // PORT 번호

	// bind(): IP주소와 PORT번호 할당
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	// listen(): 소켓이 연결요청을 받아들일 수 있는 상태가 됨
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	clnt_addr_size = sizeof(clnt_addr);

    // accept(): 연결요청의 수락을 위해 호출. 연결이 없을 때 호출되면 요청이 있을 때까지 함수는 반환되지 않음
	clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
	
	if(clnt_sock == -1)
		error_handling("accept() error");
	
    // write(): 데이터를 전송
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}