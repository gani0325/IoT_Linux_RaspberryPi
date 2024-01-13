// POSIX 메시지 큐로 메시지 출력하자 Server
#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>

int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;
    const char* name = "/posix_msq"; 		/* 메시지 큐끼리 공유할 이름 */
    char buf[BUFSIZ];
    int n;

    /* 메시지 큐 속성의 초기화 */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUFSIZ;
    attr.mq_curmsgs = 0;
    mq = mq_open(name, O_CREAT | O_RDONLY, 0644, &attr);

    /* 메시지 큐에서 데이터를 가져온다. */
    while(1) {
        n = mq_receive(mq, buf, sizeof(buf), NULL);
    switch (buf[0]) {
        /* 메시지가 ‘q’ 이면 END로 이동 */
        case 'q':
            goto END;
            break;
        /* 다른 메시지이면 화면에 가져온 데이터를 출력한다. */
        default:
            write(1, buf, n);
            break;
        }
    }

END: 			/* 메시지 큐를 닫은 후 정리하고 프로그램을 종료한다. */
    mq_close(mq);
    mq_unlink(name);

    return 0;
}

/*
서버를 작성한다
서버에서 통신을 위한 메시지 큐를 mq_open() 함수를 통해 만들고,
mq_receive() 함수로 데이터를 받아온다
가져온 데이터를 화면에 출력하고, "q" 가 들어온 경우에 메시지 큐를 닫은 후 정리하고 프로그램 종료

rt(Realtime) 라이브러리를 함께 링크해야한다
gani@gani:~/raspi/Process_Signal $ gcc -o posix_msg_server posix_msg_server.c -lrt
gani@gani:~/raspi/Process_Signal $ ./posix_msg_server &
*/