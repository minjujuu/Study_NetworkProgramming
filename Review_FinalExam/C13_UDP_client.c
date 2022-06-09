

// UDP Client
#define BUFSIZE 100
int main(int argc, char* argv[])
{
    int clnt_sock;

    char message[BUFSIZE];
    clnt_sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr, from_addr;
    socklen_t adr_sz;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    while(1) {
        fputs("Insert Message(q to quit) : ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }

        // sendto는 보내는 메시지의 크기를 전달 : strlen(message)
        // - 크기만 전달해도 되서 sizeof(serv_addr)로 함
        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        adr_sz = sizeof(from_addr);
        // recvfrom은 받을 수 있는 최대 데이터 크기를 전달 : BUFSIZE
        // - 크기의 주소값을 전달해야해서 &adr_sz
        str_len = recvfrom(clnt_sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &adr_sz);
        message[str_len] = 0;
        printf("Message from server: %s\n", message);
    }
    
    close(sock);
    return 0;
}