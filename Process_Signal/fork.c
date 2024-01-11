// fork() 함수로 프로세스를 생성하자
#include <stdio.h>
#include <unistd.h>

static int g_var = 1; 	/* data 영역의 초기화된 변수 */
char str[ ] = "PID";

int main(int argc, char **argv)
{
    int var; 		/* stack 영역의 지역 변수 */
    pid_t pid;

    var = 92;

    if((pid = fork()) < 0) { 	/* fork( ) 함수의 에러 시 처리 */
        perror("[ERROR] : fork()");
    } else if(pid == 0) { 	/* 자식 프로세스에 대한 처리 */
        g_var++; 		/* 변수의 값 변경 */
        var++;
        printf("Parent %s from Child Process(%d) : %d\n",
                                     str, getpid(), getppid());
    } else { 			/* 부모 프로세스에 대한 처리 */
        printf("Child %s from Parent Process(%d) : %d\n", str, getpid(), pid);
        sleep(1);
    }

    /* 변수의 내용을 출력 */
    printf("pid = %d, Global var = %d, var = %d\n", getpid(), g_var, var);

    return 0;
}

/*
전역 변수와 지역 변수를 한 개 생성하고, fork() 함수 이용해서 2개의 프로세스를 생성한다
fork() 함수를 수행하면 프로세스는 2개의 프로세스로 분할되어서 실행된다
pid_t 의 반환값으로 0이면 자식, 양수면 부모 프로세스이다

부모 프로세스는 자식 프로세스와 부모 프로세스 자신의 pid 를 출력하는데, 자식 프로세스의 PID 는 fork() 함수에서 반환하는 값 이용함
부모 프로세스에서는 1초간 자식 프로세스의 종료를 기다린 후 앞의 변수들을 그대로 출력 후 부모 프로세스는 종료된다
부모 프로세스는 자식 프로세스보다 먼저 종료하면 안되는데, 부모 프로세스가 먼저 종료되는 경우에는 자식 프로세스는 고아 프로세스가 된다
시스템에서 주기적으로 고아 프로세스를 찾아서 init 프로세스의 자식으로 포함시켜준다

자식 프로세스는 변수의 값들을 1씩 증가시킨 후 부모 프로세스와 자신의 pid 를 출력하는데, 부모 프로세스의 PID 구하기 위해 getppid() 사용한다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o fork fork.c
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./fork
*/