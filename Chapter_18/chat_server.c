// 서버는 중계 역할만 하고 클라이언트끼리 채팅하는 프로그램
// 다중화를 fork가 아닌 thread를 통해 구현

// gcc chat_server.c -o chat_server.out -lpthread
// 동적으로 pthread 라이브러리를 링크하기 위해 위와 같이 컴파일 해야 함 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

// Global Variable 
// - 스레드가 전역변수에 접근하는 경우 mutex를 통해 lock을 걸어주어야 함 
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if(argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // mutex를 하기 위해서는 init을 해주어야 함 
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("172.20.250.211");
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1) 
        error_handling("listen() error");

    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx); // Lock
        // 클라이언트 소켓 번호를 배열에 계속 넣어줌
        // - A라는 클라이언트가 메시지를 보내면 나머지 클라이언트에게 뿌려주어야 하므로!
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx); // Unlock

        // 클라이언트 연결 시 서비스 제공을 위한 스레드 생성 
        // - 세 번째 파라미터가 스레드가 할 일, 네 번째 파라미터가 스레드와 주고 받는 데이터
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        printf("Thread ID : %ld \n", t_id);

        pthread_detach(t_id); // 붙어있는 스레드들을 떼는 작업 
        printf("Connected client IP: %s, Client PORT: %d \n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
    }

    close(serv_sock);
    return 0;
}

// Thread가 해야 하는 일
// 매개변수인 arg를 통해 클라이언트의 소켓 번호를 받는다.
void * handle_clnt(void * arg) 
{
    int clnt_sock = *((int*)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];

    memset(msg, 0, sizeof(msg));
    // TCP이고 위에 connect()를 통해 연결되었기 때문에 read로도 가능함 (read나 recv나 동일해짐)
    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        // 보내는 쪽에서 fgets로 엔터키를 포함해서 보내기 때문에 마지막을 NULL로 변경함 
        msg[strlen(msg) - 1] = '\0';
        printf("recv : %s \n", msg);
        str_len = strlen(msg);
        // 스레드가 모든 클라이언트에게 메시지를 전송함 
        send_msg(msg, str_len);
        memset(msg, 0, sizeof(msg));
    }

    pthread_mutex_lock(&mutx);
    // 클라이언트 개수만큼 반복
    for(i = 0; i <clnt_cnt; i++) // remove disconnected client
    {
        // 클라이언트가 disconnect되었으면 지우라는 것 
        if(clnt_sock == clnt_socks[i])
        {
            while(i++ < clnt_cnt -1)
            {
                clnt_socks[i] = clnt_socks[i+1];
            }
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx); // Lock
    
    // 클라이언트 개수만큼 전송함 
    for(i=0; i<clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    
    pthread_mutex_unlock(&mutx); // Unlock
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
