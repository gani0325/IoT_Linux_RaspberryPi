#include <stdio.h>
#include <signal.h> 		/* signal( ) 함수를 위해 사용 */
#include <stdlib.h> 		/* exit( ) 함수를 위해 사용 */
#include <string.h> 		/* strsignal() 함수를 위해 사용 */
#include <unistd.h>

static void printSigset(sigset_t *set); 	/* 현재 sigset_t에 설정된 시그널 표시 */
static void sigHandler(int); 			/* 시그널 처리용 핸들러 */

int main(int argc, char **argv)
{
    sigset_t pset; 		/* 블록할 시그널을 등록할 sigset_t 형 */

    sigemptyset(&pset); 	/* 모두 0으로 설정 */
    sigaddset(&pset, SIGQUIT); 	/* SIGQUIT와 SIGRTMIN을 설정 */
    sigaddset(&pset, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &pset, NULL); 	/* 현재의 시그널 마스크에 추가 */

    printSigset(&pset); 	/* 현재 설정된 sigset_t를 화면으로 출력 */

    if(signal(SIGINT, sigHandler) == SIG_ERR) { 	/* SIGINT의 처리를 위한 핸들러 등록 */
        perror("signal() : SIGINT");
        return -1;
    }

    if(signal(SIGUSR1, sigHandler) == SIG_ERR) { 	/* SIGUSR1 처리를 위한 핸들러 등록 */
        perror("signal() : SIGUSR1");
        return -1;
    }

    if(signal(SIGUSR2, sigHandler) == SIG_ERR) { 	/* SIGUSR2 처리를 위한 핸들러 등록 */
        perror("signal() : SIGUSR2");
        return -1;
    }

    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) { 		/* SIGPIPE 처리를 위한 핸들러 등록 */
        perror("signal() : SIGPIPE");
        return -1;
    }

    while(1) pause(); 		/* 시그널 처리를 위해 대기 */

    return 0;
}

static void sigHandler(int signo) 		/* 시그널 번호를 인자로 받는다. */
{
    if(signo == SIGINT) { 			/* SIGINT 시그널이 발생했을 때 처리 */
        printf("SIGINT is catched : %d\n", signo);
        exit(0);
    } else if(signo == SIGUSR1) { 		/* SIGUSR1 시그널이 발생했을 때 처리 */
        printf("SIGUSR1 is catched\n");
    } else if(signo == SIGUSR2) { 		/* SIGUSR2 시그널이 발생했을 때 처리 */
        printf("SIGUSR2 is catched\n");
    } else if(signo == SIGQUIT) {
        printf("SIGQUIT is catched\n");
        sigset_t uset;
        sigemptyset(&uset);
        sigaddset(&uset, SIGINT);
        sigprocmask(SIG_UNBLOCK, &uset, NULL);
    } else {
        fprintf(stderr, "Catched signal : %s\n", strsignal(signo));
    }
}

static void printSigset(sigset_t *set)
{
    int i;
    for(i = 1; i < NSIG; ++i) { 		/* sigset_t에 설정된 전체 비트를 출력 */
        printf((sigismember(set, i))?"1":"0");
    }
    putchar('\n');
}

/*
하나의 sigHandler 핸들러 생성 + signal 함수 이용해서 여러 시그널을 처리
SIGINT 는 Ctrl + C 키 누르면 발생
SIGUSR1 과 SIGUSR2 는 사용자가 발생하는 시그널

시그널은 애플리케이션에 이벤트를 전달하기 위해 사용된다
사용자 시그널인 SIGUSR1 과 SIGUSR2 는 kill 명령어 사용해서 전달한다
kill 명령어에 전달하려는 시그널 번호를 직접 입력하고 싶은 경우에는 -s 옵션 사용한다

백그라운드 모드로 실해되고 있는 애플리케이션은 키보드 입력 받을 수 없음
fg 명령어 이용해서 포그라운드 모드로 전환하여 애플리케이션을 새로 포그라운드에서 실행한 후
Ctrl + C 키보드 누르면 SIGINT 시그널 전달해서 원하는 처리 수행할 수 있다

SIGQUIT 와 SIGRTMIN 시그널을 sigprocsmask) 함수 통해서 블록 처리
ISGPIPE 함수는 signal() 함수 통해서 무시하도록 처리
3개의 시그널이 발행하더라도 애플리케이션은 종료되지 않고 별다른 반응 보이지 않는다

슈퍼 유저(root) 는 임의의 프로세스에게 시그널 전달 가능
일반 유저의 경우에는 시그널을 발생하는 프로세스의 실제/유효 사용자 ID가
시그널을 수신할 프로세스의 실제/유효 사용자 ID 와 동잏한 경우에만 시그널 전달 가능

시그널 전달되면 정해진 방법대로 처리, 발생된 시그널이 전달되지 못하면 지연(pending) 상태 발생
시그널이 블록되면 시그널의 블록이 해지되거나 무시하도록 변경될 때까지 지연된 상태로 남아 있음
블록된 시그널은 시그널 마스크에 의해서 관리됨

gani@gani:~/raspi/Process_Signal $ ./handleSignal &
[2] 9803
0010000000000000000000000000000001000000000000000000000000000000
gani@gani:~/raspi/Process_Signal $ kill -USR1 9803
SIGUSR1 is catched
gani@gani:~/raspi/Process_Signal $ kill -USR2 9803
SIGUSR2 is catched
gani@gani:~/raspi/Process_Signal $ ./handleSignal
0010000000000000000000000000000001000000000000000000000000000000
^CSIGINT is catched : 2
gani@gani:~/raspi/Process_Signal $ kill -s SIGPIPE 9803
gani@gani:~/raspi/Process_Signal $ kill -s SIGQUIT 9803
gani@gani:~/raspi/Process_Signal $ kill -s SIGTERM 9803
[2]+  종료됨               ./handleSignal

*/