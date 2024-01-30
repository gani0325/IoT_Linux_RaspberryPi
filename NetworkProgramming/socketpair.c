// socketpair() 함수를 이용해서 두 프로세스 사이에 데이터를 교환한다 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    int ret, sock_fd[2];
    int status;
    char buf[ ] = "Hello World", line[BUFSIZ];
    pid_t pid;
    // socketpair() 함수는 2개의 소켓 디스크립터를 연다
    // 2개의 int 형 소켓 디스크립터를 위한 배열을 생성한다
    // 로컬 도메인만 사용 하므로 AF_LOCAL 혹은 AF_UNIX
    // 스트림 통신을 위한 SOCKET_STREAM
    ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_fd); /* 한 쌍의 소켓을 생성 */
    if(ret == -1) {
        perror("socketpair()");
        return -1;
    }

    printf("sorket 1 : %d\n", sock_fd[0]); 		/* 각 소켓의 디스크립트 번호를 출력 */
    printf("sorket 2 : %d\n", sock_fd[1]);

    if((pid = fork()) < 0) { 				/* fork( ) 함수 실행 에러 시의 처리 */
        perror("fork()");
    } else if(pid == 0) { 				/* 자식 프로세스일 때의 처리 */
        write(sock_fd[0], buf, strlen(buf) + 1); 	/* 부모 프로세스로 데이터 보내기 */
        printf("Data send : %s\n", buf);
        close(sock_fd[0]); 				/* 소켓 닫기 */
    } else { 						/* 부모 프로세스일 때의 처리 */
        wait(&status); 					/* 자식 프로세스의 종료 대기 */
        read(sock_fd[1], line, BUFSIZ); 		/* 자식 프로세스에서 온 데이터 읽기 */
        printf("Received data : %s\n", line);
        close(sock_fd[1]); 				/* 소켓 닫기 */
    }

    return 0;
}

/*
gani@gani:~/raspi/NetworkProgramming $ gcc -o socketpair socketpair.c
gani@gani:~/raspi/NetworkProgramming $ ./socketpair
*/