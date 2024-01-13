// semaphore 라이브러리로 세마포어 사용하기
#include <stdio.h> 		/* printf() 함수를 위한 헤더 파일 */
#include <fcntl.h> 		/* O_CREAT, O_EXEC 매크로를 위한 헤더 파일 */
#include <unistd.h>
#include <semaphore.h> 		/* sem_open(), sem_destroy(), sem_wait() 등 함수를 위한 헤더 파일 */

sem_t *sem; 			/* 세마포어를 위한 전역 변수 */
static int cnt = 0; 		/* 세마포어에서 사용할 임계 구역 변수 */

void p() 			/* 세마포어의 P 연산 */
{
    cnt--;
    sem_post(sem);
}

void v() 			/* 세마포어의 V 연산 */
{
    cnt++;
    sem_wait(sem);
}

int main(int argc, char **argv)
{
    const char* name = "posix_sem";
    unsigned int value = 8; 	/* 세마포어의 값 */

    /* 세마포어 열기 */
    sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, value);
    while(1) {
        if(cnt >= 8) {
            p();
            printf("decrease : %d\n", cnt);
            break;
        } else {
            v();
            printf("increase : %d\n", cnt) ;
            usleep(100);
        }
    }

    /* 다 쓴 세마포어 닫고 정리 */
    sem_close(sem);
    printf("sem_destroy return value : %d\n", sem_destroy(sem));

    /* 세마포어 삭제 */
    sem_unlink(name);

    return 0;
}

/*
phread 라이브러리를 링크한다
시스템 V의 세마포어와 같은 결과가 나온다

gani@gani:~/raspi/Process_Signal $ gcc -o posix_sem posix_sem.c -pthread
gani@gani:~/raspi/Process_Signal $ ./posix_sem 
*/