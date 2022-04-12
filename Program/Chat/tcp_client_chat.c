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
    char temp[100];
    char data[100];

    // 1. socket
    client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client_sock == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    printf("client socket = %d \n", client_sock);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(server_PORT));
    server.sin_addr.s_addr = inet_addr(server_IP);

    // 2. connect
    if (connect(client_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("connect error");
        exit(1);
    }

    while (1) // Send, Receive loop
    { 
        memset(data, 0, sizeof(data));
        printf("Client: ");
        fgets(data, sizeof(data), stdin); // input from keyboard

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
    close(client_sock);
    printf("Konkok Univ. 201914302 Park Minju\n");

    return 0;
}
