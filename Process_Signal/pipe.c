// 파이프를 생성해서 자식 프로세스와 부모 프로세스 사이에서 간단히 메시지를 전달하는 프로그램
#include <stdio.h> 		/* printf( ) 함수를 위해 사용 */
#include <unistd.h>
#include <sys/wait.h> 		/* waitpid() 함수를 위해 사용 */

int main(int argc, char **argv)
{
    pid_t pid;
    int pfd[2];
    char line[BUFSIZ]; 		/* <stdio.h> 파일에 정의된 버퍼 크기로 설정 */
    int status;

    if(pipe(pfd) < 0) { 	/* pipe( ) 함수를 이용해서 파이프 생성 */
        perror("pipe()");
        return -1;
    }

    if((pid = fork()) < 0) { 	/* fork( ) 함수를 이용해서 프로세스 생성 */
        perror("fork()");
        return -1;
    } else if(pid == 0) { 	/* 자식 프로세스인 경우의 처리 */
        close(pfd[0]); 		/* 읽기를 위한 파일 디스크립터 닫기 */
        dup2(pfd[1], 1); 	/* 쓰기를 위한 파일 디스크립터를 표준 출력(1)으로 변경 */
        execl("/bin/date", "date", 0); 		/* date 명령어 수행 */
        close(pfd[1]); 		/* 쓰기를 위한 파일 디스크립터 닫기 */
        _exit(127);
    } else { 			/* 부모 프로세스인 경우의 처리 */
        close(pfd[1]); 		/* 쓰기를 위한 파일 디스크립터 닫기 */
        if(read(pfd[0], line, BUFSIZ) < 0) { 	/* 파일 디스크립터로부터 데이터 읽기 */
            perror("read()");
            return -1;
        }
        printf("%s", line); 	/* 파일 디스크립터로부터 읽은 내용을 화면에 표시 */
        close(pfd[0]); 		/* 읽기를 위한 파일 디스크립터 닫기 */
        waitpid(pid, &status, 0); 		/* 자식 프로세스의 종료를 기다리기 */
    }

    return 0;
}

/*
pipe() 함수를 이용해 파이프를 먼저 생성한 후 fork() 함수를 이용해서 자식 프로세스를 생성한다
자식 프로세스에서 부모 프로세스로 현재 시간에 대한 메시지를 보낸다
자식 프로세스에서는 읽기를 위한 파일 디스크립터를 pfd[0] 로 설정한 후 이를 닫았고,
부모 프로세스에서는 쓰기를 위한 파일 디스크립터로 pfd[1] 로 설정한 후 닫았다

자식 프로세스의 pfd[1] 을 통해서 메시지를 보내면 부모 프로세스의 pfd[0] 을 통해서 메시지를 받을 수 있다

ls 나 data 명령어의 출력 결과는 표준 출력 (stdout) 통해서 출력된다
표준 출력으로 출력되는 내용을 프로그램 내에서 입력으로 사용하기 위해서는 표준 출력을 다른 파일 디스크립터로 돌리고
이 파일 디스크립터를 사용하면 된다 (유닉스에서 dup, dup2() 함수를 제공함)

date 명령어를 수행하면 표준 출력으로 출력되는 내용이 자식 프로세스의 파일 디스크립터 pfd[1] 으로 들어가서 파이프를 통해서
부모 프로세스의 파일 디스크립터 pfd[0] 에 전달된다
파일 디스크립터 pfd[0] 을 읽으면 자식 프로세스에서 보낸 메시지를 읽을 수 있다
파이프의 통신을 위한 작업이 완료되면 해당 파일 디스크립터를 닫아주면 된다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o pipe pipe.c
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./pipe
*/