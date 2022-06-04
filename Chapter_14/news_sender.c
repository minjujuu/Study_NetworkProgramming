#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define TTL 100 // Time to live (default는 64인데 그냥 한 번 바꿔봄)
#define BUF_SIZE 30
void error_handling(char *message);

int main(int args, char *argv[])
{
    int send_sock;
    struct sockaddr_in mul_adr;
    int time_live = TTL;
    FILE *fp;
    char buf[BUF_SIZE];

    int temp, optlen, state;
    if(argc != 3) {
        printf("Usage : %s <GroupIP> <PORT> \n", argv[0]);
        exit(1);
    }

    // Create UDP socket
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    // 데이터를 전송할 주소정보 설정. IP주소는 반드시 멀티캐스트 주소로 설정해야 함
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]); // Multicast IP
    mul_adr.sin_port = htons(atoi(argv[2]));      // Multicast Port

    // setsockopt을 통해 TTL 정보를 지정. (Sender에서 반드시 해야할 일)
    state = setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));
    if (state)
        error_handling("setsockopt() error!");

    if ((fp = fopen("_StudyMulticast.txt", "r")) == NULL) // read only
        error_handling("fopen() error");

    // 실제 데이터 전송이 이뤄지는 영역
    while (!feof(fp)) /* Multicasting */
    {
        fgets(buf, BUF_SIZE, fp); // 최대 30바이트까지 보냄 
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));
        sleep(2);
    }
 
    // close(fp);
    close(send_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}