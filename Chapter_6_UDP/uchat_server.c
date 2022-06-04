#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;
    char message[BUF_SIZE];

    int str_len;
    socklen_t clnt_adr_sz;

    char ipbuffer[32] = {0};
    struct sockaddr_in serv_adr, clnt_adr;
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(serv_sock == -1)
        error_handling("UDP socket creation error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        // serv_sock에 할당된 주소로 전달되는 모든 데이터를 수신하고 있다. 데이터의 전달대상에는 제한이 없다.
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        inet_ntop(AF_INET, &(clnt_adr.sin_addr), ipbuffer, 32);
        // 메시지를 전송한 클라이언트의 IP주소와 PORT 번호 출력 
        // printf("Client IP address : %s\n", ipbuffer);
        // printf("Client Port : %d\n", ntohs(clnt_adr.sin_port));
        message[str_len] = 0;
        // 클라이언트에서 보낸 메시지 출력
        printf("Message from Client( %d ): %s \n", ntohs(clnt_adr.sin_port), message);
        
        // 서버에서도 해당 클라이언트에 보낼 메시지 입력 
        fputs("Insert message (q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        
        // recvfrom()을 통해 데이터를 전송한 이의 주소로 채팅 메시지 전송
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
