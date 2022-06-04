#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
int global = 6;
int main()
{
    pid_t childId;
    int num = 10;

    // create pipe channel
    int pipeChannel[2] = {0};
    int pipeChannel2[2] = {0}; // 파이프는 단방향 통신이기 때문에 부모,자식이 메시지를 주고 받으려면 두 개의 파이프 필요

    for (int i = 0; i < 2; i++)
    {
        printf("BEFORE | pipe channel [%d] = %d \n", i, pipeChannel[i]);
        printf("BEFORE | pipe channel [%d] = %d \n", i, pipeChannel2[i]);
    }

    // 아래 코드 이후 pipeChannel에는 3,4가 들어오는데
    // 이미 open 되어 있는 0,1,2 빼고 다음 file descripter가 할당된 것임
    if (pipe(pipeChannel) < 0)
    {
        perror("pipe error");
        exit(1); // 1이면 비정상 종료 (0이면 정상 종료)
    }
    if (pipe(pipeChannel2) < 0)
    {
        perror("pipe error");
        exit(1); // 1이면 비정상 종료 (0이면 정상 종료)
    }

    for (int i = 0; i < 2; i++)
    {
        printf("AFTER | pipe channel [%d] = %d \n", i, pipeChannel[i]);
        printf("AFTER | pipe channel [%d] = %d \n", i, pipeChannel2[i]);
    }

    childId = fork();

    if (childId == 0)
    { // child
        //printf("child pid = %d, ppid = %d \n", getpid(), getppid());
        
        
        char buf[80];
        memset(buf, 0, sizeof(buf)); // clear
        printf("BEFORE | read to pipeChannel \n");
        close(pipeChannel[1]); // 1번 채널은 읽기 용도만 쓸 것 
        // read는 읽을 데이터가 없어야 깨어나므로 누가 먼저 파이프에 쓰느냐느느 상관없음ㅊ 
        read(pipeChannel[0], buf, sizeof(buf));
        printf("AFTER | read to pipe \n");
        printf("child pipeChannel[0] to read : %s\n", buf);
        
        strcat(buf, "_CHILD");
        write(pipeChannel2[1], buf, strlen(buf));
        close(pipeChannel2[0]);
        printf("child write to pipeChannel2[1] %s\n", buf);
        sleep(2);
        printf("child exit \n");
    
        exit(0);
    }
    else if (childId < 0)
    { // error
        perror("fork error");
        exit(1);
    }

    // parent
    //printf("parent pid = %d, parent ppid = %d, child id = %d \n", getpid(), getppid(), childId);

    // parent ==> child send "korea"
    // 0번은 받기(읽기), 1번은 보내기(쓰기)

    char temp[80];
    strcpy(temp, "korea");
    write(pipeChannel[1], temp, strlen(temp));
    close(pipeChannel[0]); // parent를 해당 채널을 write용도로만 쓰므로 

    printf("parent \"korea\" write to pipeChannel[1] ==> child \n");

    memset(temp, 0, sizeof(temp));
    close(pipeChannel2[1]); // 2번 channel은 1번 용도로만 쓰므로
    read(pipeChannel2[0], temp, sizeof(temp));
    printf("parent read to pipeChannel2[0]: %s\n", temp);
    sleep(10);
    printf("parent exit \n");

    exit(0); // 프로세스 종료

    return 0; // 함수를 콜한 곳으로 돌아감

}