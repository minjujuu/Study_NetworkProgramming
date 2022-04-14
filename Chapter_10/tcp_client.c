// TCP/IPv4 Client Program
#include <stdio.h>      // printf
#include <stdlib.h>     // exit()
#include <sys/types.h>  // socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h>     // close()
#include <string.h>     // memset
#include <arpa/inet.h>  // inet_addr()

int main(int argc, char *argv[])
{
    int client_sock;
    struct sockaddr_in server;

    int n = 0;

    char *server_IP = "";
    char *server_PORT = "30000";

    char data[100];
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

    if (connect(client_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("connect error");
        exit(1);
    }

    printf("connect OK \n");

    while (1)
    {
        printf("Client: ");
        memset(data, 0, sizeof(data));
        fgets(data, sizeof(data), stdin); // input from keyboard

        data[strlen(data) - 1] = '\0';
        // 3. send
        n = send(client_sock, data, strlen(data), 0);
        if (n != strlen(data))
        {
            perror("send error");
            exit(1);
        }

        // 4. recv
        memset(temp, 0, sizeof(temp));
        n = recv(client_sock, temp, sizeof(temp), 0);

        if (n == -1)
        {
            perror("recv error");
            exit(1);
        }
        printf("Client Recv: %s \n", temp);
    }

    // stpcpy(data, "Hello Server~~~");

    return 0;
}