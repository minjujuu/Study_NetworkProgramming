#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> // IPPROTO
void error_handling(char *message);

/** 소켓의 옵션
 * - 소켓 생성 시 한 번 결정되면 변경이 불가능하다!
 * - 소켓 생성 시 기본적으로 입력버퍼와 출력버퍼가 생성되며, 관련 옵션은 SO_SNDBUF, SO_RCVBUF 이다 
 *  */

/** getsockopt
 * #include <sys/socket.h>
 * - 소켓 옵션 참조(Get) 시 사용
 * - 성공 시 0, 실패 시 -1 반환
 * int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
 * @param sock: 옵션확인을 위한 소켓의 파일 디스크립터
 * @param level: 확인할 옵션의 프로토콜 레벨
 * @param optname: 확인할 옵션의 이름
 * @param optval: 확인결과의 저장을 위한 버퍼 주소값
 * @param optlen: 네 번째 매개변수 optval로 전달된 주소값의 버퍼크기를 담고있는 변수의 주소값
 *                함수호출 후에는 이 변수에 네 번째 인자를 통해 반환된 옵션 정보의 크기가 바이트 단위로 계산되어 저장됨
 * */

/** setsockopt
 * #include <sys/socket.h>
 * - 소켓 옵션 변경 시 사용
 * - 성공 시 0, 실패 시 -1 반환
 * int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen);
 * @param sock: 옵션변경을 위한 소켓의 파일 디스크립터
 * @param level: 변경할 옵션의 프로토콜 레벨
 * @param optname: 변경할 옵션의 이름
 * @param optval: 변경할 옵션정보를 저장할 버퍼의 주소 값 전달
 * @param optlen: 네 번째 매개변수 optval로 전달된 옵션 정보의 바이트 단위 크기 
 * 
 * */

/** sock_type.c 설명 
 * 1. 소켓의 타입정보(TCP or UDP) 확인
 * - 프로토콜 레벨: SOL_SOCKET
 * - 옵션 이름: SO_TYPE
 * 2. 소켓의 입력버퍼 및 출력버퍼 관련 옵션
 * - 프로토콜 레벨: SOL_SOCKET
 * - 옵션 레벨: SO_RCVBUF - 입력버퍼의 크기와 관련된 옵션
 * - 옵션 레벨: SO_SNDBUF - 출력버퍼의 크기이ㅘ 관련된 옵션 
 * */
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
	
	printf("SOCK_STREAM: %d \n", SOCK_STREAM); // 1
	printf("SOCK_DGRAM: %d \n", SOCK_DGRAM); // 2
	
	printf("IPPROTO_TCP: %d \n", IPPROTO_TCP); // 6
 	printf("IPPROTO_UDP: %d \n", IPPROTO_UDP); // 7
	
	// check TCP
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_sock: Socket type one: %d\n", sock_type); // 1
	
	// SO_SNDBUF: 출력 버퍼의 크기 확인 
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_sock: SO_SNDBUF %d\n", temp);
	
	//SO_RCVBUF: 입력 버퍼의 크기 확인 
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_RCVBUF: %d\n", temp);
	
	state = getsockopt(tcp_sock, IPPROTO_TCP, SO_KEEPALIVE, (void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_KEEPALIVE: %d\n", temp);
	
	state = getsockopt(tcp_sock, IPPROTO_TCP, SO_DONTROUTE, (void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("tcp_socck: SO_DONTROUTE: %d\n", temp);
	
	// ---------------------------------------
	// check UDP
	state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&temp, &optlen);
	if(state)
		error_handling("getsockopt() error!");
	printf("udp_sock: Socket type one: %d\n", temp);
	
	// SO_SNDBUF
	state = getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&temp, &optlen);
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
	// 입출력 버퍼는 상당히 주의 깊게 다루어야 하며, 코드에서 수정한 게 완벽하게 반영되진 않음
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
