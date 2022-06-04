/* 클라이언트는 바인드를 하지 않았음.
이전까지는 클라이언트가 Sender의 역할을 했기 때문.

멀티캐스트 패킷은 클라이언트가 수신자가 되기 때문에 반드시 바인드가 되어 있어야 함
Sender일 땐 바인드가 안되어있어도 상대방의 IP/PORT만 알아도 됨
상대방에 메시지를 보내면 상대방이 내 IP/PORT를 알게 되고, 그럼 상대방도 나에게 데이터 전송 가능

멀티캐스트는 내가 Sender가 아니라 Receiver가 되기 때문에
어떤 패킷을 내가 처음 수신해야하는 입장이면 반드시 바인드가 되어 있어야 함 */

// 다른 클라이언트를 또 띄우려면 multicast주소를 다르게 해야 함 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr; // ip_mreq: ip_multicast request

    if(argc != 3) {
        printf("Usage: %s <GroupIP> <PORT> \n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // multicast는 UDP만 가능

    memset(&adr, 0, sizeof(adr));
    // 1.bind, multiplcast
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: 주소들 중 아무거나
    adr.sin_port = htons(atoi(argc[2]));

    // bind 해야 함
    //- 멀티캐스트 패킷은 send가 아니라 receive를 먼저하는 경우 무조건 socket에 bind되어 있어야 함
    if (bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");

    // 2. multicast group join
    // sender에서 지정한 멀티캐스트 IP의 그룹에 참여해야 함
    // 가입해야 패킷을 수신할 수 있음
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    // 3. setsockpot
    // 멀티캐스트 그룹에 조인했다는 사실을 알리기 위해 setsockopt
    // - 이 소켓은 멀티캐스트 패킷을 받는다는 것을 나타냄
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr));

    while (1)
    {
        // recvfrom을 통해 멀티캐스트 데이터 수신 
        // - 데이터를 전송한 호스트의 주소정보를 알 필요가 없다면, 다섯 번째와 여섯번째의 인자로 각각 NULL과 0을 전달하면 된다.
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;

}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}