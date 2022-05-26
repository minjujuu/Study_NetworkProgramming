/* 클라이언트는 바인드를 하지 않았음.
이전까지는 클라이언트가 Sender의 역할을 했기 때문.

멀티캐스트 패킷은 클라이언트가 수신자가 되기 때문에 반드시 바인드가 되어 있어야 함
Sender일 땐 바인드가 안되어있어도 상대방의 IP/PORT만 알아도 됨
상대방에 메시지를 보내면 상대방이 내 IP/PORT를 알게 되고, 그럼 상대방도 나에게 데이터 전송 가능

멀티캐스트는 내가 Sender가 아니라 Receiver가 되기 때문에
어떤 패킷을 내가 처음 수신해야하는 입장이면 반드시 바인드가 되어 있어야 함 */

// 다른 클라이언트를 또 띄우려면 multicast주소를 다르게 해야 함
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[]) // Server
{
    // ----------- Multicast --------------
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    char servMessage[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr; // ip_mreq: ip_multicast request
    // -------------------------------------
    // ------------ TCP Server -------------
    int serv_sd, clnt_sd;
    FILE *fp;

    char tcpBuf[BUF_SIZE];
    char temp[100];
    int read_cnt;
    int pid = 0;

    int returnValue = 0;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 1. create socket
    serv_sd = socket(PF_INET, SOCK_STREAM, 0);

    if (serv_sd == -1)
    {
        printf("socket error \n");
        exit(1);
    }

    // printf("server socket = %d\n", serv_sd);

    // 우선 TCP 는 바인드까지만 해놓음
    // -------------------------------------
    // --------------------- Multicast Create UDP socket --------------------
    recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // multicast는 UDP만 가능

    memset(&adr, 0, sizeof(adr));
    // 1.bind, multiplcast
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: 주소들 중 아무거나
    // adr.sin_addr.s_addr = "192.168.111.128"  // a,b,c 클래스로 나누어지는 게 아니라 virtual ip라서 안되는 듯
    adr.sin_port = htons(5000);

    // bind 해야 함
    //- 멀티캐스트 패킷은 send가 아니라 receive를 먼저하는 경우 무조건 socket에 bind되어 있어야 함
    if (bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");

    // 2. multicast group join
    // sender에서 지정한 멀티캐스트 IP의 그룹에 참여해야 함
    // 가입해야 패킷을 수신할 수 있음
    join_adr.imr_multiaddr.s_addr = inet_addr("224.1.1.2");
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    // 3. setsockpot
    // 멀티캐스트 그룹에 조인했다는 사실을 알리기 위해 setsockopt
    // - 이 소켓은 멀티캐스트 패킷을 받는다는 것을 나타냄
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr));

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        printf("recvfrom: %s\n", buf);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);

        memset(servMessage, 0, sizeof(servMessage));
        strcpy(servMessage, "konkook.txt, OK, 127.0.0.1 30000");
        sendto(recv_sock, servMessage, strlen(servMessage), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);

        break;
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 2. bind
    returnValue = bind(serv_sd, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if (returnValue == -1)
    {
        perror("bind error");
        exit(1);
    }
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);

        // 4. accept
        clnt_sd = accept(serv_sd, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        if (clnt_sd == -1)
        {
            continue;
        }
        else
        {
            printf("new client connected... IP = %d, PORT = %d\n", clnt_adr.sin_addr.s_addr, clnt_adr.sin_port);
        }

        // create process
        pid = fork();
        if (pid == -1)
        {
            // 연결에 실패했으면 해당 클라이언트에게 할당된 fd를 클로즈.
            close(clnt_sd);
            perror("fork error");
            continue;
        }

        if (pid == 0) // child
        {
            close(serv_sd);
            //  서버의 소스 파일을 클라이언트에게 전송하기 위해 파일을 오픈.
            fp = fopen("Konkook.txt", "rt");

            // fp 가 NULL인 경우
            if (fp == NULL)
            {
                printf("파일이 없습니다.\n");
                write(clnt_sd, "파일이 없습니다.\n", strlen("파일이 없습니다.\n"));
                close(clnt_sd); // 클라이언트와의 연결을 끊음
            }
            else
            {
                // printf("fopen에 성공했습니다.\n");
            }
            // 파일이 있는 경우
            while (1)
            {

                // 파일로부터 읽어옴
                memset(buf, 0, sizeof(buf));
                read_cnt = fread((void *)buf, 1, BUF_SIZE, fp);
                if (read_cnt < BUF_SIZE)
                {
                    // printf("Number of characters has been read = %i\n", read_cnt);
                    // printf("buffer = %s\n", buf);
                    write(clnt_sd, buf, read_cnt);
                    break;
                }
                else
                {
                    if (ferror(fp))
                    {
                        perror("Error reading myfile");
                    }
                    else if (feof(fp))
                    {
                        perror("EOF found");
                    }
                    // printf("파일의 내용이 BUF SIZE보다 큽니다.\n");
                }
                write(clnt_sd, buf, BUF_SIZE);
                fclose(fp);
            }

            shutdown(clnt_sd, SHUT_WR);
            read(clnt_sd, buf, BUF_SIZE);
            printf("Message from client: %s \n", buf);

            close(clnt_sd);

            puts("client disconnected...");
        }
        else
        {
            close(clnt_sd);
        }
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}