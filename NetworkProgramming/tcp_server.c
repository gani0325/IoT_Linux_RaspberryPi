// TCP 소켓 기반의 통신
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100 				/* 서버의 포트 번호 */

int main(int argc, char **argv)
{
    int ssock; 					/* 소켓 디스크립트 정의 */
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr; 	/* 주소 구조체 정의 */
    char mesg[BUFSIZ];

    /* 서버 소켓 생성 */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 주소 구조체에 주소 지정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT); 	/* 사용할 포트 지정 */

    /* bind 함수를 사용하여 서버 소켓의 주소 설정 */
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* 동시에 접속하는 클라이언트의 처리를 위한 대기 큐를 설정 */
    if(listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    clen = sizeof(cliaddr);
    do {
        /* 클라이언트가 접속하면 접속을 허용하고 클라이언트 소켓 생성 */
        int n, csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);

        /* 네트워크 주소를 문자열로 변경 */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client is connected : %s\n", mesg);
        if((n = read(csock, mesg, BUFSIZ)) <= 0)
            perror("read()");
        printf("Received data : %s", mesg);

        /* 클라이언트로 buf에 있는 문자열 전송 */
        if(write(csock, mesg, n) <= 0)
            perror("write()");
        close(csock); 			/* 클라이언트 소켓을 닫음 */
    } while(strncmp(mesg, "q", 1));

    close(ssock); 			/* 서버 소켓을 닫음 */

    return 0;
}

/*
서버는 클라이언트로부터 데이터를 받아서 그대로 보내주는 TCP 기반의 에코 서버
socket() 함수에 SOCK_STREAM 옵션을 사용하여 TCP 용 소켓 디스크립터를 연다

UDP와 마찬가지로 해당 프로그램을 서버로 사용하기 위해서는 bind() 함수를 이용하여 운영체제에 서비스를 등록해야 하고,
bind() 함수의 두 번째 인자로 서버 자신의 IP 주소를 사용해야 한다

쉽게 처리하기 위해 INADDR_ANY 를 사용하였는데, 서버의 IP 주소를 자동으로 찾아서 채워준다
클라이언트의 통신을 위한 포트 번호도 설정하였다

클라이언트의 소켓들을 위해 대기 큐를 설정해야 한다. listen() 함수를 이용해서 최대 8대의 클라이언트가 동시 접속 대기를 할 수 있도록 설정하였다
소켓 큐의 설정이 끝나면 클라이언트의 접속을 기다려야 하는데, accept() 함수를 사용할 수 있다

접속한 클라이언트에 대한 정보는 두 번째 인자로 전달되며, inet_ntop() 등의 함수로 사람이 읽을 수 있는 문자 기반의 주소 스트림으로 변환해서 출력할 수 있다
실제 클라이언트의 통신은 accpet() 함수로 반환되는 소켓 디스크립터를 이용하고 read() / recv() 와 write() / send() 같은 함수를 이용해서 통신을 진행할 수 있다

통신 작업이 끝나면 close() 함수로 접속된 클라이언트와 서버의 소켓을 각각 닫는다
*/