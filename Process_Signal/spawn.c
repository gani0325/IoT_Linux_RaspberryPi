// posix_spawan() 함수는 system() 함수처럼 간단히 프로그램을 실행할 수 있음
#include <stdio.h>
#include <sys/wait.h>
#include <spawn.h>

extern char **environ;

int system(char *cmd) 		/* posix_spawn() 함수를 사용 */
{
    pid_t pid;
    int status;

    char *argv[] = {"sh", "-c", cmd, NULL};
    posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    waitpid(pid, &status, 0);

    return status;
}

int main(int argc, char **argv, char **envp)
{
    while( *envp) 		/* 환경 변수를 출력한다. */
        printf( "%s\n", *envp++);

    system("who"); 		/* who 유틸리티 수행 */
    system("nocommand"); 	/* 오류사항의 수행 */
    system("cal"); 		/* cal 유틸리티 수행 */

    return 0;
}

/*
fork() & exec() 을 사용하는 것보다 posix_spawn() 함수를 사용하는 것을 강력히 권장함
system() 함수를 posix_spawn() 함수로 만들면 간단하게 작성할 수 있음

posix_spawn() 함수의 첫 번째 인자로 fork() 함수와 같이 프로세스를 구분할 수 있는 pid_t
두 번째 인자로 exec() 함수와 같이 실행할 프로그램의 경로를 넣어준다
세 번째, 네 번째 인자를 이용해서 posix_spawn() 함수의 속성을 넣는다 (기본값으로 NULL)
다섯 번째 인자는 두 번째 인자로 넣어서 실행할 프로그램에서 사용하는 인자
마지막으로는환경 변수

실행하면 FORK() & exec() 와 동일하게 동작된다
posix_spawn() 함수의 속성 등을 이용하면 waitpid() 함수 없이 비동기적 프로그래밍을 할 수 있다

gani@gani:~/raspi/Process_Signal $ gcc -o spawn spawn.c
gani@gani:~/raspi/Process_Signal $ ./spawn 
*/