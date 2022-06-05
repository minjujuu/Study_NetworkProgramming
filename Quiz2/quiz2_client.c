/** [Client]
 * 1. TCP 소켓으로 파일 요청
 * 2. UDP 소켓으로 서버와 채팅
 * 3. UDP 소켓으로 멀티캐스트 송신
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#define TTL 100 // Time to live (default는 64인데 그냥 한 번 바꿔봄)
#define BUF_SIZE 100
void error_handling(char *message);

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
    int state = 0;
    int addNum = 0;
    int str_len = 0;
    int serv_port = 0;
    int time_live = TTL;
    int sock, chat_sock;

    socklen_t adr_sz;
    struct sockaddr_in mul_adr, serv_adr, from_adr;

    char buf[BUF_SIZE];    
    char addNumStr[30];
    char answer[BUF_SIZE];
    char ipbuffer[32] = {0};
    char new_file_name[30] = "konkuk";
    char default_file_name[30] = "konkuk2.txt";

    if (argc != 4)
    {
        printf("Usage : %s <GroupIP> <PORT 1> <PORT 2>\n", argv[0]);
        exit(1);
    }

    fputs("시나리오를 선택하세요. (1 or 2): ", stdout);
    fgets(answer, sizeof(answer), stdin);
    printf("선택한 시나리오는 %s 입니다\n", answer);
    
    // Create UDP socket
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    // 데이터를 전송할 주소정보 설정. IP주소는 반드시 멀티캐스트 주소로 설정해야 함
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]); // Multicast IP
    mul_adr.sin_port = htons(atoi(argv[2]));      // Multicast Port

    // setsockopt을 통해 TTL 정보를 지정. (Sender에서 반드시 해야할 일)
    state = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));
    if (state)
        error_handling("setsockopt() error!");

    // 받을 서비스에 대한 정보 먼저 보냄
    sendto(sock, answer, strlen(answer), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));

    if (atoi(answer) == 1)
    {
        // 파일 요청
        memset(buf, 0, sizeof(buf));
        strcat(buf, "konkuk.txt, ");
        strcat(buf, argv[1]);
        strcat(buf, ", ");
        strcat(buf, argv[2]);
        sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));

        memset(buf, 0, sizeof(buf));
        // 멀티캐스트 서버로부터 파일이 있다는 응답을 받으면 출력
        adr_sz = sizeof(serv_adr);
        str_len = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&serv_adr, &adr_sz);
        inet_ntop(AF_INET, &(serv_adr.sin_addr), ipbuffer, 32);
        printf("Server IP address : %s\n", ipbuffer);
        serv_port = ntohs(serv_adr.sin_port);
        printf("Server Port : %d\n", serv_port);
        buf[str_len] = 0;
        printf("Message from Server: %s \n", buf);

        // 이후에는 TCP 소켓이 진행되어야 함
        int tcpsocket;
        FILE *fp;

        char filebuf[BUF_SIZE];
        int read_cnt;

        // 파일 내용을 수신하여 저정할 파일 생성
        // 서버가 전송하는 파일 데이터를 담기 위해서 파일 생성
        strcpy(new_file_name, default_file_name);
        // 같은 이름의 파일이 이미 존재하는 경우 새로운 이름의 파일을 생성
        while (checkIfFileExists(new_file_name) == 1)
        {
            memset(new_file_name, 0, strlen(new_file_name));
            strcpy(new_file_name, "konkuk");
            sprintf(addNumStr, "%d", addNum++);
            strcat(new_file_name, addNumStr);
            strcat(new_file_name, ".txt");
        }

        fp = fopen(new_file_name, "wt");
        // tcp 소켓 생성
        tcpsocket = socket(PF_INET, SOCK_STREAM, 0);
        if (tcpsocket == -1)
        {
            printf("socket error \n");
            exit(1);
        }

        // 연결할 서버의 IP와 포트번호는 위에 recvfrom을 통해 진행되어 생략 가능

        if (connect(tcpsocket, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        {
            error_handling("connect() error");
        };
        while ((read_cnt = read(tcpsocket, filebuf, BUF_SIZE)) != 0)
        {
            printf("file received:: %s\n", filebuf);
            fwrite((void *)filebuf, 1, read_cnt, fp);
        }
        printf("file save in %s\n", new_file_name);
        fclose(fp);
        close(tcpsocket);
    }
    // 시나리오 2
    else
    {
        printf("-------------------- service 2 ------------------ \n");
        chat_sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (chat_sock == -1)
            error_handling("socket() error");
        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_adr.sin_port = htons(atoi(argv[3]));

        while (1)
        {

            fputs("Insert message (q to quit): ", stdout);
            fgets(buf, sizeof(buf), stdin);

            // 서버로 message를 전송
            sendto(chat_sock, buf, strlen(buf), 0, (struct sockaddr *)&serv_adr, sizeof(serv_adr));

            if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
                break;
            // 데이터를 전송한 이의 주소 정보를 저장할 구조체의 크기 계산
            adr_sz = sizeof(from_adr);

            // sock으로 전송된 message를 수신하고 데이터를 전송한 이의 주소 정보를 얻어옴
            str_len = recvfrom(chat_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&from_adr, &adr_sz);
            buf[str_len] = 0;
            printf("Message from server: %s \n", buf);
        }

        close(sock);
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}