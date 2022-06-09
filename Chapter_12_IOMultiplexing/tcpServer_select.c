#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char * buf);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock; // socket
    struct sockaddr_in serv_adr, clnt_adr;
    // select
    struct timeval timeout;
    fd_set reads, cpy_reads;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc != 2) {
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    // 1. create socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    printf("serv_sock = %d\n", serv_sock);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    FD_ZERO(&reads); // reads = { }
    FD_SET(serv_sock, &reads); // 서버 소켓을 관찰 대상으로 등록 // reads에 server sock 추가 // reads = { 3 }
    fd_max = serv_sock; // 가장 큰 파일 디스크립터의 수 fd_max = 3

    while(1)
    {
        // fd_set의 원본을 유지하기 위한 복사 과정 
        // - 3은 serv_sock인데 클라이언트가 접속했을 때 3이 생기는 거라 클라이언트를 돌려야만 진행됨
        cpy_reads = reads; //cpy_reads = {3} 
        
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
    
        puts ("before select ");
        // - select의 첫번째 매개변수는 현재 열려있는 file descriptor의 최대 개수
        if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1) {
            break;
        }

        puts ("after select ");
        if(fd_num == 0) // time-out
            continue;
        
        // select에서 0이나 -1 외의 숫자가 반환된 경우 (= 관찰 대상에 변화가 있었을 경우)
        for(int i=0; i<fd_max+1; i++)
        {
            // 변화가 있는 파일 디스크립터를 탐색 
            if(FD_ISSET(i, &cpy_reads))
            {
                // 변화가 있는 파일 디스크립터가 서버 소켓이면 클라이언트로부터 연결 요청이 있었던 것이므로
                if(i == serv_sock) // connection request !
                {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);

                    FD_SET(clnt_sock, &reads); // reads = {3}
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    
                    printf("connected client: %d\n", clnt_sock);
                } 
                else {
                    str_len = read(i, buf, BUF_SIZE);
                    if(str_len == 0){ // close request!
                        FD_CLR(i, &reads);
                        close(i);
                    } else {
                        write(i, buf, str_len); // echo!
                    }
                }
            }
        }
    }
}


void error_handling(char * buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}