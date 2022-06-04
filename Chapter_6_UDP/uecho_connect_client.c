#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

// uecho_client.c를 기반으로 connected UDP socket 으로 재구성
int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz; // 불필요해짐

    struct sockaddr_in serv_adr, from_adr; // from_adr 불필요해짐
    if(argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    // TCP 처럼 connect()를 호출했다고 해서 연결 설정이 되는 건 아니고, UDP 소켓의 목적지의 IP와 PORT정보가 등록될 뿐이다!
    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    while(1)
    {
        fputs("Insert message (q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        // 서버로 message를 전송
        // - 앞에서 connect를 통해 목적지 설정을 해주었기 때문에 데이터 전송의 과정만 거치게 된다.
        // - 소켓의 목적지 정보에 IP, PORT를 등록하거나 전송 이후 다시 목적지 정보를 삭제하는 과정을 생략할 수 있다.
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        // 데이터를 전송한 이의 주소 정보를 저장할 구조체의 크기 계산
        adr_sz = sizeof(from_adr);
        // sock으로 전송된 message를 수신하고 데이터를 전송한 이의 주소 정보를 얻어옴 
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    
    close(sock);
    return 0;
}


void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
