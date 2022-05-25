// TCP Server (UDP는 여기서 구조 달라짐)
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h> // select 
#include <sys/select.h> // select 
#include <sys/types.h> // select

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;
	
	FD_ZERO(&reads); // empty set
	FD_SET(0, &reads); // reads = { 0 }; // 0 is standard input(console)
	
	while(1)
	{
		temps = reads; // temps = { 0 }
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

        /* select 
		   - timeout이 0이면 select가 들어오기 전까지 계속 blocking 되어 있음을 의미
           - 지금은 sec이 5로 되어 있어서 5초 동안만 blocking되고 그 이후에 작업이 안들어오면 넘어감
           - timeout이면 0을 리턴, error이면 -1을 리턴, 그 외의 값을 리턴하는 경우 
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
            // 여기에서는 select의 반환값인 result가 select의 개수가 됨 (첫번째 매개변수)
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