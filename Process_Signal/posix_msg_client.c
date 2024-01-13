// POSIX 메시지 큐로 메시지 출력하자 Client
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>

int main(int argc, char **argv)
{
    mqd_t mq;
    const char* name = "/posix_msq";
    char buf[BUFSIZ];
    mq = mq_open(name, O_WRONLY);

    /* "Hello, World!"라는 문자열을 보낸다. */
    strcpy(buf, "Hello, World!\n");
    mq_send(mq, buf, strlen(buf), 0); 	/* 메시지 큐로 데이터를 보낸다. */

    /* "q"로 설정하고 보낸다. */
    strcpy(buf, "q");
    mq_send(mq, buf, strlen(buf), 0);

    /* 메시지 큐를 닫는다. */
    mq_close(mq);

    return 0;
}

/*
메시지 큐 클라이언트는 메시지 큐를 새로 생성하는 것이 아니라 기존에 존재하고 있는 메시지 큐를 연다
mq_send() 함수를 이용해서 메시지를 보내는데, 처음에는 문자열을 보내고 다시 "q" 를 보내서 서버를 종료한다
메시지 큐의 사용이 끝나면 mq_close() 함수로 메시지 큐를 닫지만, 서버와는 다르게 메시지 큐를 삭제하지 않는다

rt(Realtime) 라이브러리를 함께 링크해야한다
gani@gani:~/raspi/Process_Signal $ gcc -o posix_msg_client posix_msg_client.c -lrt
gani@gani:~/raspi/Process_Signal $ ./posix_msg_client 
*/