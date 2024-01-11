// vfork() 로 프로세스를 생성해보자
#include <stdio.h>
#include <unistd.h>

static int g_var = 1; 	/* 초기화된 data 영역의 전역 변수 */
char str[ ] = "PID";

int main(int argc, char **argv)
{
    int var; 		/* 스택(stack) 영역의 자동 변수 */

    pid_t pid;
    var = 88;

    if((pid = vfork()) < 0) { 	/* fork() 함수 호출 에러 */
        perror("vfork()");
    } else if(pid == 0) { 	/* 자식 프로세스 */
        g_var++; 		/* 변수의 값 변경 */
        var++;
        printf("Parent %s from Child Process(%d) : %d\n",
        str, getpid(), getppid());
        printf("pid = %d, Global var = %d, var = %d\n", getpid(), g_var, var);
        _exit(0);
    } else { 			/* 부모 프로세스 */
        printf("Child %s from Parent Process(%d) : %d\n", str, getpid(), pid);
    }

    printf("pid = %d, Global var = %d, var = %d\n", getpid(), g_var, var);

    return 0;
}

/*
vfork() 함수는 자식 프로세스를 생성하고 부모 프로세스는 대기하고 있기 때문에
자식 부터 출력되고, 부모 내용이 출력된다

자식 프로세스에서 변경한 전역 변수와 지역 변수의 내용이 모두 변경되어 있다
두 프로세스가 같은 공간을 사용하기 때문에 fork() 와 다르게 자식 프로세스의 변경이 부모에도 영향을 준다
자식 프로세스는 _exit() 함수를 통해서 종료해야 하는데, return 문으로 종료되는 경우 프로세들 사이에 공유된 함수의
스택도 삭제되므로 부모 프로세스의 종료 시 에러를 출력한다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o vfork vfork.c
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./vfork 
*/