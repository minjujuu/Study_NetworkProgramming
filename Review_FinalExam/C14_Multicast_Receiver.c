#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    int recv_sock;
    int str_len;

    char buf[BUF_SIZE];
    struct sockaddr _in adr;
    struct ip_mreq join_adr;

    recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    // string인 경우 inet_addr()을 쓰고, INADDR_ANY는 htonl() 쓰면 됨
    adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1) {
        // 에러
    }

    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));

    while(1)
    {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE, 0, NULL, 0);
        if(str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(recv_sock);
    return 0;

}