/** IO Multiplexing 
 * - 멀티플렉싱은 최소한의 물리적 요소만 사용해서 최대한의 데이터 전달하기 위해 사용하는 기술이다.
 * - 이전에 서버 다중화를 위해 fork()를 했었는데, 프로세스를 만드는 경우 메모리 복사가 일어나면서 자원을 더 사용할 수 밖에 없다.
 * - 그리고 프로세스의 개수가 늘어날수록 context switching이 많이 일어나게 되므로 성능에도 영향을 미칠 수 있다.
 * - 또한 프로세스는 독립적인 메모리를 가지고 있어서 서로 통신하려면 IPC를 사용해야 하는 단점이 있다.
 * */

/** select()
 * - select 함수를 사용하면 프로세스를 만들지 않고도 여러 클라이언트에 서비스를 제공할 수 있다.
 * - 여러 개의 file descriptor를 모아두고 한 번에 관찰할 수 있기 때문이다.
 * - 수신, 전송, 예외 세 가지 그룹에 대해 관찰한다.
 * 
 * - fd_set형 변수를 활용하게 되는데 여러 매크로 함수를 통해 컨트롤할 수 있다.
 * - FD_ZERO(fd_set * fdset): fd_set형 변수의 모든 비트를 0으로 초기화한다.
 * - FD_SET(int fd, fd_set * fdset): fd_set형 변수에서 fd의 비트를 1로 설정한다. (관찰대상 등록)
 * - FD_CLR(int fd, fd_set * fdset): fd_set형 변수에서 fd의 비트를 0으로 설정한다. (관찰대상 해제)
 * - FD_ISSET(int fd, fd_set * fdset): fd_set형 변수에서 fd의 비트가 1인지 확인한다. (주로 select함수 호출 이후에 결과를 확인하기 위해 사용)
 * 
 * 주의할 점
 * - select 함수를 사용 시 초기의 관찰 대상을 등록되었던 fd_set형 변수를 그대로 매개변수로 전달하면 안된다.
 * - select 함수의 매개변수로 전달된 fd_set변수에는 모든 비트를 0으로 초기화 한 후 변화가 있는 file descriptor의 비트만 1로 설정해주기 때문이다.
 * - 따라서 select함수의 매개변수로는 원래의 fd_set의 사본인 다른 변수를 사용하는 것이 좋다.
 * */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main() {

    fd_set reads, temps;
    struct timeval timeout;

    int result, str_len;
    
    char buf[BUF_SIZE];

    FD_ZERO(&reads);
    FD_SET(0, &reads);

    
    while(1) {

        temps = reads;
        
        // timeval 변수의 값은 select 호출 이후에는 타임아웃까지 남은 시간에 대한 정보가 남으므로 while 문 안에서 적어주어야 함 
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // select 호출
        result = select(1, &temps, 0, 0, &timeout);
        if(result == -1)
        {
            puts("select() error");
        } 
        else if(result == 0)
        {
            puts("time-out !");
        } else {
            if(FD_ISSET(0, &temps)) {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;

                printf("message from console : %s\n", buf);
            }
        }    
    }

    return 0;
}