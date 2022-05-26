#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);


int main(int argc, char *argv[]) {
    int serv_sd, clnt_sd;
    FILE *fp;

    char buf[BUF_SIZE];
    char temp[100];
    int read_cnt;
    int pid = 0;

    int returnValue = 0;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    if(argc != 2) {
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

    //printf("server socket = %d\n", serv_sd);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

// 2. bind
    returnValue = bind(serv_sd, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if(returnValue == -1) {
        perror("bind error");
        exit(1);
    }
    //printf("server bind success!\n");

	// 3. listen
    returnValue = listen(serv_sd, 5); 
    if(returnValue == -1) {
        perror("listen error");
        exit(1);
    }
	//printf("server listen success!\n");

	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);

        // 4. accept
		clnt_sd = accept(serv_sd, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
		if (clnt_sd == -1)
		{
			continue;
		} else {
			printf("new client connected... IP = %d, PORT = %d\n", clnt_adr.sin_addr.s_addr, clnt_adr.sin_port);
		}

		// create process
		pid = fork();
		if(pid == -1)
		{
			// 연결에 실패했으면 해당 클라이언트에게 할당된 fd를 클로즈.
			close(clnt_sd);
            perror("fork error");
			continue;
		}

		if(pid == 0) // child 
		{
			close(serv_sd);
			memset(temp, 0, sizeof(temp));
            // 클라이언트로부터 데이터를 받는다.
            while ((returnValue = recv(clnt_sd, temp, sizeof(temp), 0)) != 0)
            {
                if (returnValue == -1)
                {
                    perror("recv error");
                }
                printf("server received : %s, strlen = %ld \n", temp, strlen(temp));
                break;
            }

	     
            //  서버의 소스 파일을 클라이언트에게 전송하기 위해 파일을 오픈.
            fp = fopen(temp, "rt");

            // fp 가 NULL인 경우
            if (fp == NULL)
            {
                printf("파일이 없습니다.\n");
                write(clnt_sd, "파일이 없습니다.\n", strlen("파일이 없습니다.\n"));
                close(clnt_sd); // 클라이언트와의 연결을 끊음
            }
            else
            {
                //printf("fopen에 성공했습니다.\n");
            }
            // 파일이 있는 경우
            while (1)
            {

                // 파일로부터 읽어옴
                memset(buf, 0, sizeof(buf));
                read_cnt = fread((void *)buf, 1, BUF_SIZE, fp);
                if (read_cnt < BUF_SIZE)
                {
                    //printf("Number of characters has been read = %i\n", read_cnt);
                    //printf("buffer = %s\n", buf);
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
                    //printf("파일의 내용이 BUF SIZE보다 큽니다.\n");
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
		else {
			close(clnt_sd);
		}

	}
    close(serv_sd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}