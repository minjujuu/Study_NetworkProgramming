#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUF_SIZE 30

int checkIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else  
        return 0;
}

int main(int argc, char *argv[])
{
    int sd;
    FILE *fp;

    char buf[BUF_SIZE];
    char input[100];
    int read_cnt;
    struct sockaddr_in serv_adr;

    int returnValue = 0;
    int addNum = 0;
    char default_file_name[30] = "receive.dat";
    char new_file_name[30] = "receive";
    if(argc != 3) {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    char addNumStr[30];

    // 서버가 전송하는 파일 데이터를 담기 위해서 파일 생성
    strcpy(new_file_name, default_file_name);
    // 같은 이름의 파일이 이미 존재하는 경우 새로운 이름의 파일을 생성
    while(checkIfFileExists(new_file_name) == 1) {
        memset(new_file_name, 0, strlen(new_file_name));
        strcpy(new_file_name, "receive");
        sprintf(addNumStr, "%d", addNum++);
        strcat(new_file_name, addNumStr);  
        strcat(new_file_name, ".dat");
    }
    printf("open file name => %s\n", new_file_name);
    fp = fopen(new_file_name, "wb"); 

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd == -1) {
        printf("socket error \n");
        exit(1);
    }
    //printf("client socket = %d \n", sd);

    // 연결할 서버의 IP와 포트번호 
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    returnValue = connect(sd, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if(returnValue == -1) {
        perror("connect error");
        exit(1);
    }
    //printf("connect OK \n");

    printf("FileName: ");
    memset(input, 0, sizeof(input));
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1] = '\0'; 

    returnValue = send(sd, input, strlen(input), 0);
    if(returnValue != strlen(input))
    {
        perror("send error");
        exit(1);
    }
    //printf("Send Success! \n");

    // EOF가 전송될 때까지 데이터를 수신한 다음, 위에서 생성한 파일에 담음 
    while((read_cnt = read(sd, buf, BUF_SIZE)) != 0) {
        printf("file received:: %s\n", buf);
        fwrite((void*)buf, 1, read_cnt, fp);
    }

    //puts("Received file data");
    write(sd, "Thank you", 10);
    fclose(fp);
    close(sd);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}