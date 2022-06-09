/** Multicast
 * - UDP 는 데이터 전송 대상이 하나의 목적지였다면, Multicast는 그룹에 가입되어 있는 다수의 호스트가 목적지가 된다.
 * - 호스트들이 특정 멀티캐스트 그룹에 가입하면 Sender가 보낸 패킷을 라우터가 복사해서 각 호스트에 전달한다.
 * - 복사가 이루어지긴 하지만, 패킷을 한 번만 보내면 된다는 게 장점이다.
 * - IP주소 클래스 D (224.0.0.0 - 239.255.255.255) 에 속한다.
 * - TTL(Time to live) ; 티켓을 얼마나 멀리 전달할 것인가? 라우터를 하나 거칠 때마다 1씩 줄어들음
 *   setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void)*&time_live, sizeof(time_live));
 * - 멀티캐스트 그룹으로의 가입
 *   struct ip_mreq {
 *      struct in_addr imr_multiadd;
 *      struct in_addr imr_interface;
 *   }
 * **/

#define TTL 64
#define BUF_SIZE 30

int main(int argc, char* argv[]) {
    int send_sock;
    struct sockaddr_in mul_adr;
    int time_live = TTL;
    FILE *fp;
    char buf[BUF_SIZE];

    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]);
    mul_adr.sin_port = htons(atoi(argv[2]));

    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));
    if((fp = fopen("news.txt", "r")) == NULL) {
        // 에러
    }
    while(!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        // sendto일 때는 세번째 인자가 보낼 메시지의 크기인데 sizeof가 아닌 strlen으로 해야 함
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_adr, sizoef(mul_adr));
        sleep(2);
    }
    close(fp);
    close(send_sock);
    return 0;
}