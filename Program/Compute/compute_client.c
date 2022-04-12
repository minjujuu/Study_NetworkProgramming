// TCP/IPv4 Client Program
#include <stdio.h>      // printf
#include <stdlib.h>     // exit()
#include <sys/types.h>  // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h>     // close()
#include <string.h>     // memset
#include <arpa/inet.h>  // inet_addr()

typedef struct msg
{
    short cnt;
    short oper1;
    short oper2; // 2byte
    short oper3; // 2byte
    char op;     // 1byte
    char unuse;  // 1byte
} MSG;

typedef struct ans
{
    int sum;
} ANSWER;

// NOTE: struct size is multiple of 4

int main(int argc, char *argv[])
{
    int client_sock, accept_no;
    MSG message;
    ANSWER result;
    struct sockaddr_in server, client;

    int n, cli_len = 0;

    char *server_IP = "192.168.75.128";
    char *server_PORT = "30000";

    char temp[100];

    // 1. create TCP socket
    client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client_sock == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    printf("client_sock = %d \n", client_sock);

    memset(&server, 0, sizeof(server));            // 16byte 0 clear
    server.sin_family = AF_INET;                   // IPv4: 2byte
    server.sin_port = htons(atoi(server_PORT));    // port number: 2byte
    server.sin_addr.s_addr = inet_addr(server_IP); // 4byte

    // 2. connect
    if (connect(client_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("connect error");
        exit(1);
    }
    printf("connect OK \n");

    printf("Client: ");

    message.cnt = 2;
    message.oper1 = 23;
    message.oper2 = 35;
    message.op = '+';
    message.unuse = 0;

    printf("message.cnt = %d \n", message.cnt);
    printf("message.oper1 = %d \n", message.oper1);
    printf("message.oper2 = %d \n", message.oper2);
    printf("message.op = %c \n", message.op);

    // send's 3rd parameter
    // case 1. string - strlen(message)
    // case 2. structure - sizeof(message);
    n = send(client_sock, &message, sizeof(message), 0);
    if (n != sizeof(message))
    {
        perror("send error");
        exit(1);
    }
    printf("send success!\n");

    // 4. recv
    memset(&result, 0, sizeof(result));

    n = recv(client_sock, &result, sizeof(result), 0);
    if (n == -1)
    {
        perror("client recv error");
    }
    printf("Client Recv : %d \n", result.sum);

    return 0;
}