

/** UDP 특징
 * 1. TCP 처럼 흐름제어가 없기 때문에 신뢰성 없는 프로토콜이다.
 * 2. connect()하는 과정이 없는 대신 목적지를 매번 설정해주어야 한다.
 * 3. TCP와 달리 별 처리 없이도 여러 클라이언트에 대한 서비스가 가능하다. 
 * 4. 우편함 같이 하나의 소켓만 있으면 된다.
 * */

#define BUFSIZE 100

int main(int argc, int *argv[]) {

    int serv_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    char buf[BUFSIZE];
    socklen_t addr_sz;

    // UDP socket 생성
    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);

    // sockaddr_in 구조체 설정은 똑같이 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    // bind도 똑같이 (listen, accept는 필요 없음)
    if(bind(serv_addr, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1) {
        printf("bind error\n");
        exit(1);
    }

    while(1) {
        addr_sz = sizeof(clnt_adr);
        // recvfrom할 때에만 마지막 매개변수가 socklen_t *이고, sendto에서는 그냥 socklen_t 임 
        str_len = recvfrom(serv_sock, buf, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &addr_sz);
        sendto(serv_addr, buf, str_len, 0, (struct sockaddr*)&clnt_adr, addr_sz);
    }
    close(serv_sock);
    return 0;
}