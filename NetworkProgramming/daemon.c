// daemon.c
// 데몬 프로세스를 만들기
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>

int main(int argc, char **argv)
{
    struct sigaction sa; /* 시그널 처리를 위한 시그널 액션 */
    struct rlimit rl;
    int fd0, fd1, fd2, i;
    pid_t pid;

    if(argc < 2) {
        printf("Usage : %s command\n", argv[0]);
        return -1;
    }

    /* 파일 생성을 위한 마스크를 0으로 설정 */
    umask(0);

    /* 사용할 수 있는 최대의 파일 디스크립터 수 얻기 */
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getlimit()");
    }

    if((pid = fork()) < 0) {
        perror("error()");
    } else if(pid != 0) { /* 부모 프로세스는 종료한다. */
        return 0;
    }

    /* 터미널을 제어할 수 없도록 세션의 리더가 된다. */
    setsid();

    /* 터미널 제어와 관련된 시그널을 무시한다. */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("sigaction() : Can't ignore SIGHUP");
    }

    /* 프로세스의 워킹 디렉터리를 ‘/’로 설정한다. */
    if(chdir("/") < 0) {
        perror("cd()");
    }

    /* 프로세스의 모든 파일 디스크립터를 닫는다. */
    if(rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    for(i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    /* 파일 디스크립터 0, 1과 2를 /dev/null로 연결한다. */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /* 로그 출력을 위한 파일 로그를 연다. */
    openlog(argv[1], LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        return -1;
    }

    /* 로그 파일에 정보 수준의 로그를 출력한다. */
    syslog(LOG_INFO, "Daemon Process");
    while(1) {
        /* 데몬 프로세스로 해야 할 일을 반복 수행 */
    }

    /* 시스템 로그를 닫는다. */
    closelog();

    return 0;
}

/*
파일 생성을 위한 마스크를 umask() 함수를 통해 0으로 설정하고 새로운 프로세스를 생성한다
부모 프로세스를 종료한 후 자식 프로세스를 세션 리더로 만들어 터미널 조작을 하지 못하게 된다
터미널 제어와 관련된 시그널은 무시하고 프로세스의 현재 워킹 디렉터리를 '/' 로 설정한다

모든 파일 디스크립터를 닫고 표준 입력(0), 표준 출력(1), 표준 에러(2) 를 'dev/null' 로 연결한다
새로운 프로세스는 데몬 프로세스처럼 동작한ㄷ

데몬 프로세스는 터미널 입출력과 파일 입출력을 할 수 없기 때문에 문제 발생 시 관련 메시지를 출력할 수 없다
이 문제를 해결하기 위해, 유닉스에서는 시스템 로그 데몬이 백그라운드에서 동작한다
이 시스템 로그 데몬에 메시지를 보내면 /var/log/ 디렉터리에 관련 메시지들을 저장하는데, openlog(), syslog(), close() 함수를 이용할 수 있다

컴파일 실행할 때 실행 파일 뒤에 명령어의 이름을 입력해야 한다
프로그램의 실행을 터미널로 확인할 수 없으므로 ps 유틸리티를 이용하자

gani@gani:~/raspi/NetworkProgramming $ gcc -o daemon daemon.c
gani@gani:~/raspi/NetworkProgramming $ ./daemon ls
gani@gani:~/raspi/NetworkProgramming $ ps aux | grep daemon
*/