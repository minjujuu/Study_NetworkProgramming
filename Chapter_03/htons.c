#include <stdio.h>
#include <arpa/inet.h>

int main() 
{
	unsigned short host_port = 0x1234;
	unsigned short net_port;
	unsigned short return_host_port;
	net_port = htons(host_port);
    // Little Endian: 상위 바이트의 값을 큰 번지수에 저장
	printf("host_port: %x \n", host_port); // host_port: 1234 
	printf("host_port: %p %x \n", (char*)&host_port, *(char*) &host_port);
	printf("host_port+1: %p %x \n\n", (char*)&host_port+1, *((char*) &host_port+1));
	printf("net_port: %x \n", net_port);
	printf("net_port: %p %x \n", (char*)&net_port, *(char*) &net_port);
	printf("net_port+1: %p %x \n\n", (char*)&net_port+1, *((char*) &net_port+1));
	
	return_host_port = ntohs (net_port);
	printf("return_host_port : %x \n", return_host_port);
	return 0;

/* 출력 결과
    host_port: 1234 
    host_port: 0x7ffd633ce0a2 34 
    host_port+1: 0x7ffd633ce0a3 12 

    net_port: 3412 
    net_port: 0x7ffd633ce0a4 12 
    net_port+1: 0x7ffd633ce0a5 34 

    return_host_port : 1234 
*/
}