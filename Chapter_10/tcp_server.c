// TCP/IPv4 Server Program
#include <stdio.h>      // printf
#include <stdlib.h>     // exit()
#include <sys/types.h>  // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h>     // close()
#include <string.h>     // memset
#include <arpa/inet.h>  // inet_addr()

// 멀티프로세스 기반의 서버 구현
// - 연결이 하나 생성될 때마다 프로세스를 생성해서 해당 클라이언트에 대해 서비스를 제공
// 1단계. 에코 서버(부모 프로세스)는 accept 함수호출을 통해서 연결요청을 수락
// 2단계. 이 때 얻게 되는 소켓의 파일 디스크립터를 자식 프로세스를 생성해서 전달
// 3단계. 자식 프로세스는 전달받은 파일 디스크립터를 바탕으로 서비스를 제공

int main(int argc, char *argv[])
{
    int ser_sock, accept_no;
    struct sockaddr_in server, client;

    char temp[100];
    int pid;
    int n = 0, client_length = 0;
    char *ser_ip = "";
    char *ser_port = "30000";

    // 1. create TCP socket
    ser_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ser_sock == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    printf("server socket = %d \n", ser_sock);

    memset(&server, 0, sizeof(server));         // 16byte 0 clear
    server.sin_family = AF_INET;                // IPv4: 2byte
    server.sin_port = htons(atoi(ser_port));    // port number: 2byte
    server.sin_addr.s_addr = inet_addr(ser_ip); // 4byte

    // 2. bind
    n = bind(ser_sock, (struct sockaddr *)&server, sizeof(server));
    if (n == -1)
    {
        perror("bind error");
        exit(1);
    }
    printf("server bind success!\n");

    // 3. listen
    if (listen(ser_sock, 100) == -1)
    {
        perror("listen error");
        exit(1);
    }
    printf("server listen success!\n");

    while (1)
    {

        client_length = sizeof(client);
        accept_no = accept(ser_sock, (struct sockaddr *)&client, &client_length);
        if (accept_no == -1)
        {
            continue;
        }
        else
        {
            puts("new client connected...");
        }

        // create process
        pid = fork();
        if (pid == -1)
        {
            close(accept_no);
            continue;
        }

        if (pid == 0)
        { // client service
            close(ser_sock);
            memset(temp, 0, sizeof(temp));
            
            while ((n = recv(accept_no, temp, sizeof(temp), 0)) != 0)
            {
                strcat(temp, ":Server");
                send(accept_no, temp, strlen(temp), 0);
                memset(temp, 0, sizeof(temp));
            }
            close(accept_no);
            puts("client disconnected ...");
            return 0;
        }
        else
        {
            close(accept_no);
        }
    }
    return 0;
}