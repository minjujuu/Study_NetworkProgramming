#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> // IPPROTO
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int tcp_sock, udp_sock;
	int sock_type;
	int temp = 0;
	socklen_t optlen;
	int state;
	
	optlen = sizeof(sock_type);
	
	tcp_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	udp_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	printf("tcp_sock = %d\n", tcp_sock); // 3
	printf("udp_sock = %d\n", udp_sock); // 4
	
	printf("SOCK_STREAM: %d \n", SOCK_STREAM);
	printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);
	
	printf("IPPROTO_TCP: %d \n", IPPROTO_TCP);
	printf("IPPROTO_UDP: %d \n", IPPROTO_UDP);
	
	// check TCP
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE,
			(void*)&sock_type, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_sock: Socket type one: %d\n", sock_type);
	
	// SO_SNDBUF
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_sock: SO_SNDBUF %d\n", temp);
	
	//SO_RCVBUF
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_RCVBUF: %d\n", temp);
	
	state = getsockopt(tcp_sock, IPPROTO_TCP, SO_KEEPALIVE,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_KEEPALIVE: %d\n", temp);
	
	state = getsockopt(tcp_sock, IPPROTO_TCP, SO_DONTROUTE,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_DONTROUTE: %d\n", temp);
	
	// ---------------------------------------
	// check UDP
	state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("udp_sock: Socket type one: %d\n", temp);
	
	// SO_SNDBUF
	state = getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("udp_sock: SO_SNDBUF %d\n", temp);
	
	//SO_RCVBUF
	state = getsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF,
			(void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("BEFORE udp_sock: SO_RCVBUF: %d\n", temp);
	
	temp = 20000;
	optlen = sizeof(temp);
	/* set은 특별한 경우가 아니면 하지 말 것. get을 하는 것이 일반적. */
	state = setsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&temp, optlen);
	if(state)
		error_handling("setsockopt() error!");
	
	printf("udp_sock SO_RCVBUF : %d changed \n", temp);
	state = getsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF,
			(void*)&temp, &optlen);
			
	if(state)
		error_handling("getsockopt() error!");
		
	// 위에서 revb buf인 temp를 20000으로 바꾸었지만 40000으로 출력이 됨
	// 입출력 버퍼는 상당히 주의 깊게 다루어야 하기 때문에 코드에서 수정한 게 완벽하게 반영되진 않음
	printf("AFTER udp_sock: SO_RCVBUF: %d\n", temp); 

	state = getsockopt(udp_sock, IPPROTO_IP, IP_TTL,
			(void*)&temp, &optlen);
	printf("udp_sock: IP_TTL: %d \n", temp);	

	return 0;
		
}

void error_handling(char *message)
{

}


/*
tcp_sock = 3
udp_sock = 4
SOCK_STREAM: 1 
SOCK_DGRAM: 2 
IPPROTO_TCP: 6 
IPPROTO_UDP: 17 
tcp_sock: Socket type one: 1
tcp_sock: SO_SNDBUF 16384
tcp_socck: SO_RCVBUF: 131072
tcp_socck: SO_KEEPALIVE: 0
tcp_socck: SO_DONTROUTE: 75
udp_sock: Socket type one: 2
udp_sock: SO_SNDBUF 212992
udp_sock: SO_RCVBUF: 212992
BEFORE udp_sock: SO_RCVBUF: 212992
udp_sock SO_RCVBUF : 20000 changed 
AFTER udp_sock: SO_RCVBUF: 40000
udp_sock: IP_TTL: 64 

UDP는 Send, Recv가 TCP보다 큼 기본적으로.
- TCP는 서버가 아닌 이상 RECV할 일이 더 많아서 RCVBUF가 훨 큼

참고
- man 7 socket : 메뉴얼 보기 
*/
