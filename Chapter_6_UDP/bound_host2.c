#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char msg1[] = "Hi!";
    char msg2[] = "I'm another UDP host!";
    char msg3[] = "Nice to meet you";

    struct sockaddr_in your_adr;
    socklen_t your_adr_sz;

    if(argc != 3) {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("sock() error");
    
    memset(&your_adr, 0, sizeof(your_adr));
    your_adr.sin_family = AF_INET;
    your_adr.sin_addr.s_addr = inet_addr(argv[1]);
    your_adr.sin_port = htons(atoi(argv[2]));

    /** 여기서는 왜 my_adr에 대한 bind()가 필요 없는걸까?
    /** UDP 클라이언트 소켓의 주소정보 할당 시점에 그 이유가 있다.
         * sendto 함수 호출 이전에 되어야 하는데, bind() 함수를 통해 명시적으로 주소정보를 할당할 수 있고,
         * sendto 함수 호출 시까지 주소정보가 할당되지 않았다면, sendto 함수 호출 시점에 자동으로 소켓에 IP, PORT가 할당된다.
         * 자동으로 할당될 때의 IP는 호스트의 IP로, PORT는 사용하지 않는 PORT번호 하나를 임의로 골라서 할당하게 된다 
    * */
    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*)&your_adr, sizoef(your_adr));
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&your_adr, sizoef(your_adr));
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&your_adr, sizoef(your_adr));

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}