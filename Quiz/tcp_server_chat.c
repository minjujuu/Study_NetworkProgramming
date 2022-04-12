// TCP/IPv4 Server Program
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
    int ser_sock;
    int accept_no = 0;
    struct sockaddr_in server;
    struct sockaddr_in client;
    char temp[100];

    int n = 0, client_length = 0;
    char *ser_ip = "";
    char *ser_port = "30000";
    // 1. socket
    ser_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ser_sock == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    printf("server socket = %d \n", ser_sock);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(ser_port));
    server.sin_addr.s_addr = inet_addr(ser_ip);

    // 2. bind
    n = bind(ser_sock, (struct sockaddr *)&server, sizeof(server));
    if (n == -1)
    {
        perror("bind error");
        exit(1);
    }
    printf("server bind success\n");

    // 3. listen
    if (listen(ser_sock, 100) == -1)
    {
        perror("listen error");
        exit(1);
    }
    printf("server listen success\n");

    client_length = sizeof(client);

    // 4. accept
    accept_no = accept(ser_sock, (struct sockaddr *)&client, &client_length);
    if (accept_no == -1)
    {
        perror("accept error");
        exit(1);
    }

    while (1) // Recv, Send loop
    {                                  
        memset(temp, 0, sizeof(temp)); // init

        // 5. recv (서버는 기본적으로 recv 먼저)
        n = recv(accept_no, temp, sizeof(temp), 0);
        if (n == -1)
        {
            perror("server recv error");
        }

        printf("Server Recv : %s \n", temp);
        printf("Server: ");

        fgets(temp, sizeof(temp), stdin);

        n = send(accept_no, temp, strlen(temp), 0);
        // 6. send
        if (n != strlen(temp))
        {
            perror("send error");
            exit(1);
        }
    }

    printf("Kunkok Univ. 201914302 Park Minju\n");
    return 0;
}
