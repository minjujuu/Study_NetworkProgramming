#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
    pid_t childId;
    int num = 10;
    childId = fork();

    if(childId == 0) { // child
        printf("child pid = %d, ppid = %d \n", getpid(), getppid());
        num = num + 5;
        printf("child number = %d \n", num);
        sleep(2);
        printf("child exit \n"); 
        exit(0);
    } 
    else if(childId < 0) { // error
        perror("fork error");
        exit(1);
    }

    // parent
    printf("parent pid = %d, parent ppid = %d, child id = %d \n", getpid(), getppid(), childId);
    num = num + 25;
    printf("parent number = %d \n", num);
    sleep(10);
    printf("parent exit \n");  
    
    exit(0); // 프로세스 종료

    return 0; // 함수를 콜한 곳으로 돌아감 

    /* out
        parent pid = 178, parent ppid = 8, child id = 179
        child pid = 179, ppid = 178
        parent number = 35
        child number = 15
        child exit
        parent exit
    */

    /* 결과 정리
        코드 상에서는 똑같은 num이지만 fork하는 순간 clone 되고,
        부모와 자식은 서로 다른 주소 공간을 가지게 되기 때문에
        서로의 number 값에 영향을 줄 수 없음 
     */
}