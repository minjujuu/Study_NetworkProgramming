#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

// TCP 클라이언트와 다른 점은 connect 함수의 호출이 존재하지 않다는 점 
int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];

    int str_len;
    socklen_t adr_sz;

    struct sockaddr_in serv_adr, from_adr;
    if(argc != 3) {
        printf("Usage : %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    while(1)
    {
        fputs("Insert message (q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        /** UDP 클라이언트 소켓의 주소정보 할당은 언제될까?
         * sendto 함수 호출 이전에 되어야 하는데, bind() 함수를 통해 명시적으로 주소정보를 할당할 수 있고,
         * sendto 함수 호출 시까지 주소정보가 할당되지 않았다면, sendto 함수 호출 시점에 자동으로 소켓에 IP, PORT가 할당된다.
         * 자동으로 할당될 때의 IP는 호스트의 IP로, PORT는 사용하지 않는 PORT번호 하나를 임의로 골라서 할당하게 된다 */
        // 서버로 message를 전송
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

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}