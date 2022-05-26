#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define TTL 100 // Time to live (default는 64인데 그냥 한 번 바꿔봄)
#define BUF_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[]) // Client
{
    // ----------- Multicast --------------
    int send_sock;
    struct sockaddr_in mul_adr, from_adr;
    int time_live = TTL;
    FILE *fp;
    char buf[BUF_SIZE];
    char multicastMessage[BUF_SIZE];
    int temp, optlen, state;

    socklen_t adr_sz;
    int str_len = 0;
    // -------------------------------------

    // ------------ TCP Client -------------
    int sd;
    char input[100];
    int read_cnt;
    struct sockaddr_in serv_adr;

    int returnValue = 0;
    char default_file_name[30] = "konkook.txt";

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    fp = fopen(default_file_name, "wt");

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    // TODO: 응답 받아서 바인딩해야함
    // 연결할 서버의 IP와 포트번호
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    // -------------------------------------

    // --------------------- Multicast Create UDP socket --------------------
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&mul_adr, 0, sizeof(mul_adr));
    // 데이터를 전송할 주소정보 설정. IP주소는 반드시 멀티캐스트 주소로 설정해야 함
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr("224.1.1.2"); // Multicast IP
    mul_adr.sin_port = htons(5000);                   // Multicast Port

    optlen = sizeof(temp);

    // setsockopt을 통해 TTL 정보를 지정. (Sender에서 반드시 해야할 일)
    state = setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));
    if (state)
        error_handling("setsockopt() error!");

    strcpy(multicastMessage, "Konkook.txt 127.0.0.1 30000");

    // 실제 데이터 전송이 이뤄지는 영역
    returnValue = sendto(send_sock, multicastMessage, strlen(multicastMessage), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));
    if (returnValue != strlen(multicastMessage))
    {
        perror("send error");
        exit(1);
    }

    memset(multicastMessage, 0, sizeof(multicastMessage));
    adr_sz = sizeof(from_adr);
    str_len = recvfrom(send_sock, multicastMessage, BUF_SIZE, 0, (struct sockaddr *)&from_adr, &adr_sz);
    multicastMessage[str_len] = 0;
    printf("Message from server: %s\n", multicastMessage);
    // while (!feof(fp)) /* Multicasting */
    // {
    //     fgets(buf, BUF_SIZE, fp); // 최대 30바이트까지 보냄
    //     sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));
    //     sleep(2);
    // }
    returnValue = connect(sd, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if (returnValue == -1)
    {
        perror("connect error");
        exit(1);
    }

    // printf("Send Success! \n");

    // EOF가 전송될 때까지 데이터를 수신한 다음, 위에서 생성한 파일에 담음
    while ((read_cnt = read(sd, buf, BUF_SIZE)) != 0)
    {
        printf("file received:: %s\n", buf);
        fwrite((void *)buf, 1, read_cnt, fp);
    }

    // puts("Received file data");
    write(sd, "Thank you", 10);
    fclose(fp);
    close(sd);

    close(send_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
