// TCP 소켓 기반의 통신
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100

int main(int argc, char **argv)
{
    int ssock;
    struct sockaddr_in servaddr;
    char mesg[BUFSIZ];

    if(argc < 2) {
        printf("Usage : %s IP_ADRESS\n", argv[0]);
        return -1;
    }

    /* 소켓을 생성 */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 소켓이 접속할 주소 지정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    /* 문자열을 네트워크 주소로 변경 */
    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    /* 지정한 주소로 접속 */
    if(connect(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect()");
        return -1;
    }

    fgets(mesg, BUFSIZ, stdin);
    if(send(ssock, mesg, BUFSIZ, MSG_DONTWAIT) <= 0) { 	/* 데이터를 소켓에 쓴다. */
        perror("send()");
        return -1;
    }

    memset(mesg, 0, BUFSIZ);
    if(recv(ssock, mesg, BUFSIZ, 0) <= 0) { 		/* 데이터를 소켓으로부터 읽는다. */
        perror("recv()");
        return -1;
    }

    printf("Received data : %s", mesg); 		/* 받아온 문자열을 화면에 출력 */ 

    close(ssock); 					/* 소켓을 닫는다. */ 

    return 0;
}

/*
클라이언트에서는 TCP 용 소켓을 생성한 후 connect() 함수를 사용하여 서버에 접속한다
connect() 함수에는 서버 주소에 대한 정보를 명시한다
기본적인 주소 설정 방법은 bind() 함수와 동일하지만, connect() 함수는 INADDR_ANY 를 사용할 수 없다 (포트 번호는 서버와 같은 번호를 사용)

서버에 접속한 후 fgets() 함수를 이용해서 키보드로 데이터를 받아들인 후, 앞에서 생성한 소켓을 이용하여 send() 나 write() 함수를 통해 서버로 데이터를 보내고,
resv() 나 read() 함수를 통해 데이터를 받는다. printf() 함수를 이용해서 서버로부터 받은 데이터를 화면에 표시하였다

앞의 코드를 빌드하고 먼저 서버를 실행시킨다. UDP 와 마찬가지로 클라이언트를 함께 실행하려면 다른 터미널이 필요하므로 다른 터미널을 열고 로그인 OR 서버를 백그라운드로 ㅣㄹ행
같은 라즈베리파이/PC 에서 실행되고 있는 서버로 접속할 때는 서버의 127.0.0.1을 입력한다

클라이언트에서 문자열을 입력하면 TCP 를 통해서 클라이언트와 서버 간에 통신이 이뤄진다
클라이언트는 먼저 서버로 문자열을 보내면 서버에서는 문자열을 읽고 다시 클라이언트에게 되돌려준다
클라이언트는 서버로부터 전달된 데이터를 받아서 화면에 출력하고 서버를 종료하고 싶을 때는 클라이언틀르 시작하고 q 를 입력하면 된다
*/