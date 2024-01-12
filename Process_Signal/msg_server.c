// 메시지 큐를 이용한 통신 server
// 서버와 클라이언트는 동일한 키 값 가져야 한다 (51234 로 설정함)
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

#define MSQKEY 51234

struct msgbuf {
    long mtype; /* 메시지의 타입 : 0 이상의 정숫값 */
    char mtext[BUFSIZ]; /* 메시지의 내용 : 1바이트 이상의 문자열 */
};

int main(int argc, char **argv)
{
    key_t key;
    int n, msqid;
    struct msgbuf mb;
    key = MSQKEY;

    /* 메시지 큐의 채널을 생성한다. */
    if((msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        perror("msgget()");
        return -1;
    }

    /* 메시지 큐에서 데이터를 가져온다. */
    while((n = msgrcv(msqid, &mb, sizeof(mb), 0, 0)) > 0) {
        switch (mb.mtype) {
            /* 메시지 타입(mtype)이 1이면 화면에 가져온 데이터를 출력한다. */
            case 1:
                write(1, mb.mtext, n);
                break;
            /* 메시지 타입(mtype)이 2이면 메시지 큐의 채널을 삭제한다. */
            case 2:
                if(msgctl(msqid, IPC_RMID, (struct msqid_ds *) 0) < 0) {
                    perror("msgctl()");
                    return -1;
                }
                break;
        }
    }

    return 0;
}

/*
서버는 메시지 큐를 위한 채널을 생성하고 메시지 큐로부터 데이터를 가져와서 mtype 값이 1이면 화면에 가져온 데이터 출력
mtype 값이 2이면 메시지 큐에 대한 채널을 삭제한다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o msg_server msg_server.c
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./msg_server &
*/