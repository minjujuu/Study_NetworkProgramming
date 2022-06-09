// TCP Server (UDP는 여기서 구조 달라짐)
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h> // select 
#include <sys/select.h> // select 
#include <sys/types.h> // select

#define BUF_SIZE 30
/** 멀티플렉싱?
 * - 물리적 장치의 효율성을 노핑기 위해 최소한의 물리적인 요소만 사용해서 최대한의 데이터를 전달하기 위해 사용하는 기술
 *  IO 멀티플렉싱?
 * - 프로세스를 생성하지 않고도 다수의 클라이언트에게 서비스를 제공할 수 있는 방법
 * */

/** select 함수의 이해와 서버의 구현
 * - select 함수를 사용하면 한 곳에 여러 개의 파일 디스크립터를 모아놓고 동시에 이들을 관찰할 수 있다.
 * - 관찰중인 파일 디스크립터에 변화가 생겨야 반환하며, 변화가 생기지 않으면 무한정 블로킹 상태에 머물게 된다.
 * - select 함수 호출 이후 1로 설정된 파일 디스크립터에서 변화가 발생했다고 판단할 수 있다.
 * 
 * 관찰할 수 있는 항목들 (Event)
 *  1) 수신한 데이터를 지니고 있는 소켓이 존재하는가? (수신)
 *  2) 블로킹되지 않고 데이터의 전송이 가능한 소켓은 무엇인가? (전송)
 *  3) 예외상황이 발생한 소켓이 무엇인가? (예외)
 * */

/** select 함수의 호출 방법과 순서
 *  Step 1. 파일 디스크립터의 설정, 검사의 범위 지정, 타임아웃의 설정
 *  - 파일 디스크립터의 설정은 소켓의 관찰이기도 함 
 *  - 관찰하고자 하는 파일 디스크립터를 모을 땐, fd_set형 변수를 이용
 *  Step 2. select 함수의 호출
 *  Step 3. 호출결과 확인 
 * */

/** fd_set형 변수
 *  - 0과 1로 표현되는, 비트 단위로 이뤄진 배열
 *  - 비트가 1로 설정되면 관찰대상으로 지정을 의미 
 *  fd_set형 변수에 값을 등록하거나 변경하는 등의 작업을 도와주는 매크로 함수 
 *  1) FD_ZERO(fd_set *fdset): 인자로 전달된 주소의 fd_set형 변수의 모든 비트를 0으로 초기화
 *  2) FD_SET(int fd, fd_set *fdset): 매개변수 fdset으로 전달된 주소의 변수에 매개변수 fd로 전달된 파일 디스크립터 정보를 등록
 *  3) FD_CLR(int fd, fd_set *fdset): 매개변수 fdset으로 전달된 주소의 변수에서 매개변수 fd로 전달된 파일 디스크립터 정보를 삭제
 *  4) FD_ISSET(int fd, fd_set *fdset): 매개변수 fdset으로 전달된 주소의 변수에 매개변수 fd로 전달된 파일 디스크립터 정보가 있으면 양수를 반환 
 *   - FD_ISSET은 select 함수의 호출 결과를 확인하는 용도로 사용됨 
 * */

/** int select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);
 * @param maxfd: 검사 대상이 되는 파일 디스크립터 수 (생성 시 1개씩 늘어나기 때문에 '가장 큰 파일 디스크립터 값 + 1' 하면 됨!)
 * @param *readset: fd_set형 변수에 '수신된 데이터의 존재여부'에 관심 있는 파일 디스크립터 정보를 모두 등록해서 그 변수의 주소 값을 전달
 * @param *writeset: fd_set형 변수에 '블로킹 없는 데이터 전송의 가능여부'에 관심 있는 파일 디스크립터 정보를 모두 등록해서 그 변수의 주소 값을 전달
 * @param *exceptset: fd_set형 변수에 '예외상황의 발생여부'에 관심 있는 파일 디스크립터 정보를 모두 등록해서 그 변수의 주소 값을 전달
 * @param *timeout: select 함수 호출 이후에 무한정 블로킹 상태에 빠지지 않도록 타임아웃(time-out)을 설정하기 위한 인자를 전달 
 * */

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;
	
	FD_ZERO(&reads); // empty set
	FD_SET(0, &reads); // fd0을 비트 1로 설정해서 관찰대상으로 등록 // reads = { 0 }; // 0 is standard input(console)
	
	while(1)
	{
        /** reads를 temps에 복사하는 이유?
         * - select 함수호출이 끝나면 변화가 생긴 파일 디스크립터의 위치를 제외한 나머지 위치의 비트들은 0으로 초기화 됨
         * - 원본의 유지를 위해서는 이렇게 복사의 과정이 필요! */
		temps = reads; // temps = { 0 }

        /** 여기에서 timeout을 설정하는 이유?
         * - select 함수 호출 후에는 timeval의 tv_sec과 tv_usec의 값이 타임아웃이 발생하기까지 남았던 시간으로 바뀌기 때문
         * - 따라서 select함수를 호출하기 전에 매번 초기화를 반복해야 함! */
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

        /* select 
		   - timeout이 0이면 select가 들어오기 전까지 계속 blocking 되어 있음을 의미
           - 지금은 sec이 5로 되어 있어서 5초 동안만 blocking되고 그 이후에 작업이 안들어오면 넘어감
           - timeout이면 0을 리턴, error이면 -1을 리턴, 콘솔로부터 입력된 값이 있다면 0보다 큰 수를 반환 
           - temps에는 0(stdin)이 들어있으므로, 호출 후 block 되어있다가 키보드 입력이 되면 반환
        */
		result = select(1, &temps, 0, 0, &timeout);
		
        if(result == -1) {
            puts("select() error");
            break;
        }else if(result == 0) {
            puts("Time-out!");
        }
        else {
            // 변화를 보인 파일 디스크립터가 stdin이 맞는지 확인 
            // - select() 함수 호출 이후이므로 temps에는 변화를 보인 파일 디스크립터만 1로 설정될 것             
            if(FD_ISSET(0, &temps)) // FD_ISSET(): 0번이 temps 안에 있다면 true
            {
                str_len = read(0, buf, BUF_SIZE);
                printf("str_len = %d\n", str_len); // str_len은 입력된 글자수보다 + 1 (엔터까지)
                buf[str_len-1] = 0; // 엔터 부분을 null로 설정 
                printf("message from console: %s", buf);
            }
        }
	}
    return 0;
}