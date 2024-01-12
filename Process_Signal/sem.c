// 세마포어를 수행하기 위한 과정
// 세마포어의 P 연산과 V 연산에 대한 함수
// 두 함수를 이용해서 세마포어의 동작 알아보기
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>

int cnt = 0;
static int semid;

void p() 				/* 세마포어의 P 연산 */
{
    struct sembuf pbuf;
    pbuf.sem_num = 0;
    pbuf.sem_op = -1;
    pbuf.sem_flg = SEM_UNDO;

    if(semop(semid, &pbuf, 1) == -1) 	/* 세마포어의 감소 연산을 수행한다. */
        perror("p : semop()");
}

void v() 				/* 세마포어의 V 연산 */
{
    struct sembuf vbuf;
    vbuf.sem_num = 0;
    vbuf.sem_op = 1;
    vbuf.sem_flg = SEM_UNDO;

    if(semop(semid, &vbuf, 1) == -1) 	/* 세마포어의 증가 연산을 수행한다. */
        perror("v : semop()");
}

int main(int argc, char **argv)
{
    union semun { 			/* semun 공용체 */
        int val;
        struct semid_ds *buf;
        unsigned short int *arrary;
    } arg;

    /* 세마포어에 대한 채널 얻기 */
    if((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget()");
        return -1;
    }

    arg.val = 1; 			/* 세마포어 값을 1로 설정 */
    if(semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl() : SETVAL");
        return -1;
    }

    while(1) {
        if(cnt >= 8) {
            cnt--;
            p();
            printf("decrease : %d\n", cnt);
            break;
        } else {
            cnt++;
            v();
            printf("increase : %d\n", cnt);
            usleep(100);
        }
    }

    /* 세마포어에 대한 채널 삭제 */
    if(semctl(semid, 0, IPC_RMID, arg) == -1) {
        perror("semctl() : IPC_RMID");
        return -1;
    }

    return 0;
}

/*
세마포어를 위한 채널을 생성하고 세마포어 한 개를 생성한다
코드 내에서 세마포어에 대한 P연산과 V연산을 수행하는데,
V연산으로 세마포어가 증가되고 난 이후에 P연산을 통해 감소될 수 있다

두 연산의 실행 순서를 바꾸면 P연산에서는 사용할 수 있는 세마포어가 생길 때까지 대기한다
8개까지 세마포어가 증가한 후 다시 세마포어가 감소하는 것을 확인할 수 있다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o sem sem.c
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./sem
*/