#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char*buf);

int main(int argc, char *argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;
    if(argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 1. TCP socket 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);

    // 2. sockaddr_in 구조체 초기화
    memset(&serv_adr, 0, sizeof(serv_adr));
    // 3. serv_adr 구조체에 연결하고자 하는 서버의 IP, PORT를 설정
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_family.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    // 4. connect()를 통해 연결시도
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    
    // 5. 입출력 루틴 분할을 위한 프로세스 생성
    pid = fork();
    if(pid == 0) // 자식프로세스는 write 루틴
        write_routine(sock, buf);
    else // 부모프로세스는 read 루틴 
        read_routine(sock, buf);

    close(sock);
    return 0;

}

// 부모 프로세스가 실행할 read 루틴
// - 데이터 read에 관련된 코드만 존재 
void read_routine(int sock, char *buf)
{
    while(1)
    {
        int str_len = read(sock, buf, BUF_SIZE);
        if(str_len == 0)
            return;
        
        buf[str_len] = 0;
        printf("Message from server : %s", buf);
    }
}

// 자식 프로세스가 실행할 write 루틴
// - 데이터 write에 관련된 코드만 존재함 
void write_routine(int sock, char *buf)
{
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            /** shutdown()
             * - 서버로 EOF 전달을 위해 호출
             * - fork함수 호출을 통해 파일 디스크립터가 복사된 상황이므로, 한번의 close함수로 EOF의 전달을 기대할 수 없음
             * - 따라서 반드시 shutdown 함수 호출을 통해 EOF의 전달을 별도로 명시해야 함 */
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}