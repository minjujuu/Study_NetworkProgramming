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
    for (int i = 0; i < 2; i++)
    {
        printf("BEFORE | pipe channel [%d] = %d \n", i, pipeChannel[i]);
    }

    // 아래 코드 이후 pipeChannel에는 3,4가 들어오는데
    // 이미 open 되어 있는 0,1,2 빼고 다음 file descripter가 할당된 것임
    if (pipe(pipeChannel) < 0)
    {
        perror("pipe error");
        exit(1); // 1이면 비정상 종료 (0이면 정상 종료)
    }

    for (int i = 0; i < 2; i++)
    {
        printf("AFTER | pipe channel [%d] = %d \n", i, pipeChannel[i]);
    }

    childId = fork();

    if (childId == 0)
    { // child
        //printf("child pid = %d, ppid = %d \n", getpid(), getppid());
        
        
        char buf[80];
        memset(buf, 0, sizeof(buf)); // clear
        read(pipeChannel[0], buf, sizeof(buf));
        printf("child pipe to read : %s \n", buf);
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
    printf("parent \"korea\" write to pipe ==> child \n");

    sleep(10);
    printf("parent exit \n");

    exit(0); // 프로세스 종료

    return 0; // 함수를 콜한 곳으로 돌아감

    /* out
        parent pid = 178, parent ppid = 8, child id = 179
        child pid = 179, ppid = 178
        parent number = 35, global = 23
        child number = 15, global = 13
        child exit
        parent exit
    */

    /* 결과 정리
        코드 상에서는 똑같은 num이지만 fork하는 순간 clone 되고,
        부모와 자식은 서로 다른 주소 공간을 가지게 되기 때문에
        서로의 number 값에 영향을 줄 수 없음

        전역변수(global) 여도 서로의 값에 영향을 줄 수 없는데,
        fork 시점에 local, global 변수든 간 새로운 프로그램으로 클론되어 영향을 줄 수 없음
     */
}