// 공유 메모리
// 2개의 프로세스를 만들어서 부모 프로세스와 자식 프로세스가 같은 메모리 공간 사용하기
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>

#define SHM_KEY 0x12345 		/* 공유 메모리를 위한 키 */

int main(int argc, char **argv)
{
    int i, pid, shmid;
    int *cVal;
    void *shmmem = (void *)0;

    if((pid = fork()) == 0) { 		/* 자식 프로세스를 위한 설정 */
        /* 공유 메모리 공간을 가져온다. */
        shmid = shmget((key_t)SHM_KEY, sizeof(int), 0);
        if(shmid == -1) {
            perror("shmget()");
            return -1;
        }

        /* 공유 메모리를 사용하기 위해 프로세스의 메모리에 붙인다. */
        shmmem = shmat(shmid, (void *)0, 0666 | IPC_CREAT);
        if(shmmem == (void *)-1) {
            perror("shmat()");
            return -1;
        }

        cVal = (int *)shmmem;
        *cVal = 1;
        for(i = 0; i < 3; i++) {
            *cVal += 1;
            printf("Child(%d) : %d\n", i, *cVal);
            sleep(1);
        }
    } else if(pid > 0) { 		/* 부모 프로세스로 공유 메모리의 내용을 표시 */
        /* 공유 메모리 공간을 만든다. */
        shmid = shmget((key_t)SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
        if(shmid == -1) {
            perror("shmget()");
            return -1;
        }

        /* 공유 메모리를 사용하기 위해 프로세스의 메모리에 붙인다. */
        shmmem = shmat(shmid, (void *)0, 0);
        if(shmmem == (void *)-1) {
            perror("shmat()");
            return -1;
        }

        cVal = (int *)shmmem;
        for(i = 0; i < 3; i++) {
            sleep(1);
            printf("Parent(%d) : %d\n", i, *cVal);
        }
    }

    shmctl(shmid, IPC_RMID, 0);

    return 0;
}

/*
2개의 프로세스를 생성한 후 shmget() 함수를 이용해서 공유 메모리 대한 채널을 가져와서
shmat() 함수를 이용해 공유 메모리를 현재의 프로세스의 공간에 붙인다

자식 프로세스에서는 변수의 값을 증가시키고 이를 화면에 출력시키고 1초간 슬립 모드
부모 프로세스는 1초간 슬립 모드에 들어간 후 깨어나서 공유 메모리에 대한 내용을 출력한다
자식 프로세스에서 공유 메모리로 설정해놓은 내용을 부모 프로세스에서도 같은 메모리를 공유하여 동일한 값이 출력된다


*/