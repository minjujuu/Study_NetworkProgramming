/** [Server]
 * 1. TCP 소켓으로 파일 전송 서비스 (다중화 서비스, fork)
 * 2. UDP 소켓으로 채팅 서비스 (다중화 서비스)
 * 3. UDP 소켓으로 멀티캐스트 수신
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[]) // Server
{
    int pid = 0;
    int option = 0;
    int str_len = 0;
    int read_cnt = 0;

    // 멀티캐스트 소켓
    int serv_sock = 0; 

    // 시나리오1 소켓
    int tcpsocket = 0; 
    int clisocket = 0;

    // 시나리오2 소켓
    int udp_sock = 0; 

    FILE *fp;

    socklen_t clnt_adr_sz;
    struct ip_mreq join_adr;
    // 멀티캐스트 sockaddr
    struct sockaddr_in serv_adr;
    // 시나리오1 sockaddr
    struct sockaddr_in tcp_serv_adr;
    // 시나리오2 sockaddr
    struct sockaddr_in chat_serv_adr;
    // 공용 sockaddr 
    struct sockaddr_in clnt_adr;
    
    char temp[100];
    char buf[BUF_SIZE];
    char filebuf[BUF_SIZE];
    char ipbuffer[32] = {0};    

    if (argc != 4)
    {
        printf("Usage: %s <GroupIP> <PORT 1> <PORT 2>\n", argv[0]);
        exit(1);
    }

    // 1. create multicast socket
    serv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[2]));
    inet_ntop(AF_INET, &(serv_adr.sin_addr), ipbuffer, 32);

    option = 1;
    // 중복된 포트에 bind하는 것을 허용하도록 설정
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    printf("Server IP address : %s\n", ipbuffer);
    // 2. bind
    // receive를 먼저할 것이므로 바인드 필수
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    // 3. join multicast group
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(serv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr));

    memset(buf, 0, sizeof(buf));
    memset(ipbuffer, 0, sizeof(ipbuffer));
    // 이후에는 tcp소켓으로 파일 전송할 준비를 해야 함

    // tcp 소켓 생성
    tcpsocket = socket(PF_INET, SOCK_STREAM, 0);
    if (tcpsocket == -1)
    {
        error_handling("socket() error");
    }

    memset(&tcp_serv_adr, 0, sizeof(tcp_serv_adr));
    tcp_serv_adr.sin_family = AF_INET;
    tcp_serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_serv_adr.sin_port = htons(atoi(argv[2]));

    // bind
    if (bind(tcpsocket, (struct sockaddr *)&tcp_serv_adr, sizeof(tcp_serv_adr)) == -1)
    {
        error_handling("bind error");
    }

    while (1)
    {
        memset(&clnt_adr, 0, sizeof(clnt_adr));
        memset(buf, 0, sizeof(buf));
        clnt_adr_sz = sizeof(clnt_adr);
        // clnt_adr에 데이터를 전송한 클라이언트의 IP, PORT 번호를 불러올 수 있음
        str_len = recvfrom(serv_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        if(buf != 0)
            printf("Selected Sevice ( %d ): %s \n", ntohs(clnt_adr.sin_port), buf);

        // 시나리오 1 
        if (strstr(buf, "1") != NULL)
        {
            printf("----------- start service 1 ---------------\n");
            str_len = recvfrom(serv_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
            inet_ntop(AF_INET, &(clnt_adr.sin_addr), ipbuffer, 32);
            printf("Client IP address : %s\n", ipbuffer);
            printf("Client Port : %d\n", ntohs(clnt_adr.sin_port));
            buf[str_len] = 0;

            // 파일 전송 서비스를 할지, 채팅 서비스를 할지 정해야 할 듯
            // - 클라이언트에서 보낸 메시지 출력
            printf("Message from Client( %d ): %s \n", ntohs(clnt_adr.sin_port), buf);

            memset(buf, 0, sizeof(buf));
            // 2. 서버는 멀티캐스트 메시지를 수신한 후 클라이언트 IP, PORT를 통해
            // “konkuk.txt” 파일이 있음을 클라이언트 IP, PORT로 메시지(“konkuk.txt”, “OK“, 서버IP, PORT)를 전송한다
            strcpy(buf, "konkuk.txt, ");
            strcat(buf, "OK, ");
            strcat(buf, "127.0.0.1, ");
            sprintf(ipbuffer, "%d", ntohs(serv_adr.sin_port));
            strcat(buf, ipbuffer);
            printf("Send message: %s\n", buf);
            sendto(serv_sock, buf, strlen(buf), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);

            // listen
            if (listen(tcpsocket, 5) == -1)
            {
                error_handling("listen error");
            }

            clnt_adr_sz = sizeof(clnt_adr);
            // accept
            clisocket = accept(tcpsocket, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
            if (clisocket == -1)
            {
                continue;
            }
            else
            {
                printf("client connected...\n");
            }

            pid = fork();
            if (pid == -1)
            {
                close(clisocket);
                perror("fork error");
                continue;
            }

            if (pid == 0)
            {
                close(tcpsocket);
                fp = fopen("konkuk.txt", "rt");
                if (fp == NULL)
                {
                    printf("파일이 없습니다.\n");
                    write(clisocket, "파일이 없습니다.\n", strlen("파일이 없습니다.\n"));
                    close(clisocket); // 클라이언트와의 연결을 끊음
                }

                // 파일이 있는 경우
                while (1)
                {
                    memset(filebuf, 0, sizeof(filebuf));
                    read_cnt = fread((void *)filebuf, 1, BUF_SIZE, fp);
                    printf("file read_cnt = %d\n", read_cnt);
                    printf("file text = %s\n", filebuf);
                    if (read_cnt < BUF_SIZE)
                    {
                        write(clisocket, filebuf, read_cnt);
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
                    }
                    write(clisocket, filebuf, BUF_SIZE);
                    fclose(fp);
                }

                shutdown(clisocket, SHUT_WR);
                close(clisocket);
                puts("client disconnected...");
                return 0; // child process exit
            }
            else
            {
                close(clisocket);
            }
        }
        else if (strstr(buf, "2") != NULL)
        {
            close(serv_sock);
            printf("----------- start service 2 ---------------\n");
            udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
            option = 1;
       
            if (udp_sock == -1)
                error_handling("UDP socket creation error");

            memset(&chat_serv_adr, 0, sizeof(chat_serv_adr));
            chat_serv_adr.sin_family = AF_INET;
            chat_serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
            chat_serv_adr.sin_port = htons(atoi(argv[3]));

            if (bind(udp_sock, (struct sockaddr *)&chat_serv_adr, sizeof(chat_serv_adr)) == -1)
                error_handling("bind() error");

            while (1)
            {
                clnt_adr_sz = sizeof(clnt_adr);
                // udp_sock에 할당된 주소로 전달되는 모든 데이터를 수신하고 있다. 데이터의 전달대상에는 제한이 없다.
                str_len = recvfrom(udp_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                inet_ntop(AF_INET, &(clnt_adr.sin_addr), ipbuffer, 32);
                buf[str_len] = 0;
                // 클라이언트에서 보낸 메시지 출력
                printf("Message from Client( %d ): %s \n", ntohs(clnt_adr.sin_port), buf);
                if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
                {                  
                    break;
                }
                // 서버에서도 해당 클라이언트에 보낼 메시지 입력
                fputs("Insert message: ", stdout);
                fgets(buf, sizeof(buf), stdin);
                
                // recvfrom()을 통해 데이터를 전송한 이의 주소로 채팅 메시지 전송
                sendto(udp_sock, buf, strlen(buf), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            }
            close(udp_sock);
        }
    }
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
