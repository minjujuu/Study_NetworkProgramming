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
    int serv_sock;

    char message[BUF_SIZE];
}
void error_handling(char * buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}