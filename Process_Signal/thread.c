// 간단한 스레드 (posix 세마포어 함수를 스레드를 이용해서 변경하기)
#include <stdio.h> 		/* printf() 함수를 위한 헤더 파일 */
#include <unistd.h>
#include <fcntl.h> 		/* O_CREAT, O_EXEC 매크로를 위한 헤더 파일 */
#include <pthread.h>
#include <semaphore.h> 		/* sem_open(), sem_destroy(), sem_wait() 등 함수를 위한 헤더 파일 */

sem_t *sem; 			/* 세마포어를 위한 전역 변수 */
static int cnt = 0; 		/* 세마포어에서 사용할 임계 구역 변수 */

void p() 			/* 세마포어의 P 연산 */
{
    sem_post(sem);
}

void v() 			/* 세마포어의 V 연산 */
{
    sem_wait(sem);
}

void *ptheadV(void *arg) 	/* V 연산을 수행하기 위한 함수를 작성한다. */
{
    int i;

    for(i = 0; i < 10; i++) {
        if(cnt >= 7) usleep(100); 	/* 7 이상이면 100밀리초 동안 대기한다. */
        cnt++;
        printf("increase : %d\n", cnt) ;
        fflush(NULL);
        v();
    }

    return NULL;
}

void *ptheadP(void *arg) 	/* P 연산을 수행하기 위한 함수를 작성한다. */
{
    int i;

    for(i = 0; i < 10; i++) {
        p(); 			/* 세마포어가 0이 되면 블록된다. */
        cnt--;
        printf("decrease : %d\n", cnt);
        fflush(NULL);
        usleep(100); 		/* 100밀리초 간 기다린다. */
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t ptV, ptP; 	/* 스레드를 위한 자료형 */
    const char* name = "posix_sem";
    unsigned int value = 7; 	/* 세마포어의 값 */

    /* 세마포어 열기 */
    sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, value);
    pthread_create(&ptV, NULL, ptheadV, NULL); 	/* 스레드 생성 */
    pthread_create(&ptP, NULL, ptheadP, NULL);

    pthread_join(ptV, NULL); 	/* 스레드가 종료될 때까지 대기 */
    pthread_join(ptP, NULL);

    /* 다 쓴 세마포어 닫고 정리 */
    sem_close(sem);
    printf("sem_destroy() : %d\n", sem_destroy(sem));

    /* 세마포어 삭제 */
    sem_unlink(name);

    return 0;
}

/*
메인 스레드 이외의 2개의 스레드가 번갈아가면서 실행된다
첫 번째 스레드를 만들고 값을 증가시키고, 두 번째 스레드를 만들고 나서 값을 감소시킨다
위의 값을 증가시키는 곳에서는 7 이상이면 100밀리초 동안 정지하고,
값을 감소시키는 곳에서도 100밀리초 동안 정지한다

스레드를 사용할 때 잠시 정지하지 않고 계속 실행하게 되면 다른 스레드가 늦어질 수 있으므로
usleep() 함수를 적절히 사용해서 일의 중간에 잠시 정지하도록 하는 것이 좋다

빌드하기 위해 pthread 라이브러리를 링크해야 한다
gcc는 -pthread를 지원하고 있지만, 다른 컴파일러를 위해서 -l 옵션 이용해서 pthread 라이브러리와 링크할 수 있도록 설정한다
빌드 후 실행해보면 값이 증가하고 난 이후에 값이 계쏙해서 감소되는데 감소하는 시점에서 세마포어가 0인 경우에는
값이 증가되기 전까지 semop() 함수에서 대기하고 있는 것을 확인한다
스레드는 실행할 때마다 실행 순서가 바뀔 수 있으므로 결과값이 다르게 나올 수 있다

gani@gani:~/raspi/Process_Signal $ gcc -o thread thread.c -lpthread
gani@gani:~/raspi/Process_Signal $ ./thread 
*/