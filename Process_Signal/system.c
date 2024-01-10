// system() 함수를 구현하면서 fork(), exec(), waitpid() 함수 사용하기
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int system(const char *cmd) 	/* fork(), exec(), waitpid() 함수를 사용 */
{
    pid_t pid;
    int status;

    if((pid = fork()) < 0) { 	/* fork( ) 함수 수행 시 에러가 발생했을 때의 처리 */
        status = -1;
    } else if(pid == 0) { 	/* 자식 프로세스의 처리 */
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127); 		/* execl( ) 함수의 에러 사항 */
    } else { /* 부모 프로세스의 처리 */
        while(waitpid(pid, &status, 0) < 0) 	/* 자식 프로세스의 종료 대기 */
        if(errno != EINTR) { 	/* waitpid( ) 함수에서 EINTR이 아닌 경우의 처리 */
            status = -1;
            break;
        }
    }

    return status;
}

int main(int argc, char **argv, char **envp)
{
    while(*envp) 		/* 환경 변수를 출력한다. */
        printf("%s\n", *envp++);

    system("who"); 		/* who 유틸리티 수행 */
    system("nocommand"); 	/* 오류사항의 수행 */
    system("cal"); 		/* cal 유틸리티 수행 */

    return 0;
}

/*
fork() 함수를 이용해서 프로세스를 생성하고 execl() 함수를 이용해서 사용자가 입력한 명령을 수행한다
sh 셸 명령어로 수행할 수 있도록 /bin/sh 의 인자로 명령어를 입력하였고
부모 프로세스는 waitpid() 함수를 이용해서 자식 프로세스의 수행을 기다리도록 한다

main() 함수의 3번째 인자로 현재 설정된 환경 변수들의 목록을 가져올 수 있다
환경 변수의 사용을 위해 main() 함수의 3번째 인자를 사용하지 않고 광역 변수 environ 을 사용할 수 있다

현재 설정된 환경 변수들이 화면에 출력되고, who 명령어의 결과가 출력된 후 nocommand 라는 명령어가 없으므로 에러가 출력된다
cal 명령어에 대한 결과가 출력된다

gani@gani:~/raspi/Process_Signal $ gcc -o system system.c
gani@gani:~/raspi/Process_Signal $ ./system 
*/