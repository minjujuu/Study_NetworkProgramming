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

// Point 1. TCP 서버임
// Point 2. fd_set에 서버 소켓을 등록해두고 변화가 있으면 클라이언트가 접속했다고 판단함
// Point 3. 클라이언트 또한 같은 fd_set에 등록하고, 수신한 데이터가 있으면 echo 서비스를 진행 
int main(int argc, char *argv[]) {

    struct sockaddr_in serv_adr, clnt_adr;
    int serv_sock, clnt_sock;

    socklen_t clnt_sz;
    
    fd_set reads, cpy_reads;
    struct timeval timeout;
    int result, str_len, fd_max;
    
    char buf[BUF_SIZE];
    if(argc != 2) {
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET:
    serv_adr.sin_addr.s_add = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        error_handling("bind() error");
    } 

    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    FD_ZERO(&reads);
    // 서버 소켓을 fd_set에 등록해서 변화가 있다면 클라이언트의 연결 요청이 있는 것으로 판단 
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while(1) {

        cpy_reads = reads;

        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        result = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
        if(result == -1) {
            error_handling("select error");
        } else if(result == 0) {
            continue;
        }

        for(int i=0; i<fd_max + 1; i++) {
            // servsock 말고 다른 변화도 감지해서 처리하기 위해 for문으로 구성
            if(FD_ISSET(i, &cpy_reads)) {
                // 클라이언트 접속된 경우
                if(i == serv_sock) {
                    clnt_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_sz);
                    FD_SET(clnt_sock, &reads);

                    if(fd_max < clnt_sock)  
                        fd_max = clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                
                } else {
                    str_len = read(i, buf, BUF_SIZE);
                    // 수신한 데이터가 EOF인 경우에는 소켓을 종료 
                    if(str_len == 0) {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("Closed client : %d\n", i);
                    } else {
                        // echo !
                        write(i, buf, str_len);

                    }
                }
            } 

        }
        

    }

    close(serv_sock);
    return 0;
}

void error_handling(char * buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}