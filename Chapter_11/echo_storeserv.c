#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int fds[2]; 

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];

    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) {
        error_handling("bind() error");
    }
    
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");        
    }

    // 파이프 생성
    pipe(fds);
    // 파일의 데이터 저장을 담당할 프로세스 생성
    pid = fork();
    // child
    if(pid == 0) 
    {
        FILE * fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int i, len;

        // 파일에 데이터가 어느 정도 채워지면 파일을 닫도록
        for(i=0; i<10; i++) {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }

        fclose(fp);
        return 0;
    }

    while(1)
    {
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
        if(clnt_sock == -1) {
            continue;
        }
        else {
            puts("new client connected...");
        }
        
        pid = fork();
        if(pid == 0) {
            // 해당 자식 프로세스는 서비스 제공을 위한 프로세스이므로 서버소켓은 닫아줌
            close(serv_sock); 
            // clnt_sock으로부터 읽어온 내용이 0보다 큰 경우 계속해서 buf에 저장
            while((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
            }

            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        } else {
            // 해당 클라이언트에 대한 서비스는 자식 프로세스가 진행하고 있으므로
            // 클라이언트 소켓을 닫아줌 
            close(clnt_sock);
        }        
    }

    close(serv_sock);
    return 0;   
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
