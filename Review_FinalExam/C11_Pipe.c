
/** Pipe (파이프)
 * - 프로세스들은 독립된 메모리 공간(Code, Data, Heap, Stack) 을 갖는다.
 * - 따라서 서로 통신하려면 IPC 방법을 따로 써야 하고 그 중 하나가 파이프이다.
 * 
 * - int fds[2] 를 만들고 pipe()의 매개변수로 넣어주면 fds는 파이프라는 운영체제 자원이 된다.
 * - 운영체제 도움을 받아야 하는 이유는 서로 공유하는 메모리 공간을 따로 부여받아야 하기 때문이다.
 * 
 * - 파이프에서 fds[0]는 출력이므로 read 시 사용하고, fds[1]은 입력이므로 write 시 사용한다. 
 * - 단, 양방향으로 통신하는 경우 주의해야 할 점이 있는데, 파이프에 있는 데이터는 먼저 가져가는 사람이 임자?이기 때문이다.
 * - 프로세스 A가 write하고 바로 read하는 경우, 다른 프로세스가 가져가지 못하고 자기가 고대로 가져가는 상황이 된다.
 * - 따라서 만약 두 프로세스가 양방향 통신을 해야하는 경우 파이프 2개를 생성하는 것이 더 안정적으로 구현할 수 있다.
 *   만약 파이프1,2 가 있다면 프로세스 A는 파이프1에는 write만, 파이프2에는 read만 하는 방식이다.
 * 
 * - 그리고 파이프는 fork()에 의해 복사 대상이 아니다!
 * */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define BUF_SIZE 50

int main() {
    
    int fds1[2];
    int fds2[2];

    pid_t pid;
    char str1[] = "Hello";
    char str2[] = "I'm minju";

    char buf[BUF_SIZE];
    pipe(fds1);
    pipe(fds2);

    pid = fork();

    if(pid == 0) {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output : %s \n", buf);

    } else {
        read(fds1[0], buf, sizeof(buf));
        printf("Parent proc output : %s \n", buf);
        write(fds2[1], str2, BUF_SIZE);
        sleep(3);
    }

}