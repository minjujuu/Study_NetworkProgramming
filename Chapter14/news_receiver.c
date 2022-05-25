/* 클라이언트는 바인드를 하지 않았음.
이전까지는 클라이언트가 Sender의 역할을 했기 때문.

멀티캐스트 패킷은 클라이언트가 수신자가 되기 때문에 반드시 바인드가 되어 있어야 함 
Sender일 땐 바인드가 안되어있어도 상대방의 IP/PORT만 알아도 됨
상대방에 메시지를 보내면 상대방이 내 IP/PORT를 알게 되고, 그럼 상대방도 나에게 데이터 전송 가능

멀티캐스트는 내가 Sender가 아니라 Receiver가 되기 때문에 
어떤 패킷을 내가 처음 수신해야하는 입장이면 반드시 바인드가 되어 있어야 함 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message);

int main(int argc, char *argv[])
[
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr; //ip_mreq: ip_multicast request

    recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // multicast는 UDP만 가능
    memset(&adr, 0, sizeof(adr));

    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: 주소들 중 아무거나 
    // adr.sin_addr.s_addr = "192.168.111.128"  // a,b,c 클래스로 나누어지는 게 아니라 virtual ip라서 안되는 듯 
    adr.sin_port = htons(5000);

    // bind 해야 함
    //- 멀티캐스트 패킷은 send가 아니라 receive를 먼저하면 무조건 socket에 bind되어 있어야 함
    if(bind(recv_sock, (struct sockaddr*) &adr, sizeof(adr)) == -1)
        error_handling("bind() error");
    
    // sender에서 지정한 멀티캐스트 IP로 그룹에 참여해야 함 
    join_adr.im

]