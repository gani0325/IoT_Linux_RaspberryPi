// TCP 서버에서 select() 함수를 이용하여 병렬 처리하기
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 5100 		/* 서버의 포트 번호 */

int main(int argc, char **argv)
{
    int ssock;
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    fd_set readfd; 			/* select( ) 함수를 위한 자료형 */
    int maxfd, client_index, start_index;
    int client_fd[5] = {0}; 		/* 클라이언트의 소켓 FD 배열 */

    /* 서버 소켓 디스크립터 연다. */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr)); 	/* 운영체제에 서비스 등록 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if(bind(ssock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    if(listen(ssock, 8) < 0) { 		/* 클라이언트의 소켓들을 위한 큐 생성 */
        perror("listen()");
        return -1;
    }

    FD_ZERO(&readfd); 			/* fd_set 자료형을 모두 0으로 초기화 */
    maxfd = ssock; 			/* 현재 최대 파일 디스크립터의 번호는 서버 소켓의 디스크립터 */
    client_index = 0;

    do {
        FD_SET(ssock, &readfd); 	/* 읽기 동작 감지를 위한 fd_set 자료형 설정 */

        /* 클라이언트의 시작 주소부터 마지막 주소까지 fd_set 자료형에 설정 */
        for(start_index = 0; start_index < client_index; start_index++) {
            FD_SET(client_fd[start_index], &readfd);
            if(client_fd[start_index] > maxfd)
                maxfd = client_fd[start_index]; 	/* 가장 큰 소켓의 번호를 저장 */
        }
        maxfd = maxfd + 1;

        /* select( ) 함수에서 읽기가 가능한 부분만 조사 */
        select(maxfd, &readfd, NULL, NULL, NULL); 	/* 읽기가 가능해질 때까지 블로킹 */
        if(FD_ISSET(ssock, &readfd)) { 			/* 읽기가 가능한 소켓이 서버 소켓인 경우 */
            clen = sizeof(struct sockaddr_in);		/* 클라이언트의 요청 받아들이기 */
            int csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
            if(csock < 0) {
                perror("accept()");
                return -1;
            } else {
                /* 네트워크 주소를 문자열로 변경 */
                inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                printf("Client is connected : %s\n", mesg);

                /* 새로 접속한 클라이언트의 소켓 번호를 fd_set에 추가 */
                FD_SET(csock, &readfd);
                client_fd[client_index] = csock;
                client_index++;
                continue;
            }
            if (client_index == 5) break;
        }

        /* 읽기 가능했던 소켓이 클라이언트였던 경우 */
        for(start_index = 0; start_index < client_index; start_index++) {
            /* for 루프를 이용해서 클라이언트들을 모두 조사 */
            if(FD_ISSET(client_fd[start_index], &readfd)) {
                memset(mesg, 0, sizeof(mesg));

                /* 해당 클라이언트에서 메시지를 읽고 다시 전송(Echo) */
                if((n = read(client_fd[start_index], mesg, sizeof(mesg))) > 0) {
                    printf("Received data : %s", mesg);
                    write(client_fd[start_index], mesg, n);
                    close(client_fd[start_index]); 	/* 클라이언트 소켓을 닫는다. */

                    /* 클라이언트 소켓을 지운다. */
                    FD_CLR(client_fd[start_index], &readfd);
                    client_index--;
                }
            }
        }
    } while(strncmp(mesg, "q", 1));

    close(ssock); 					/* 서버 소켓을 닫는다. */

    return 0;
}

/*
클라이언트와의 통신을 위한 소켓을 생성 후, 운영체제에 서비스를 등록하고, 클라이언트의 접속을 처리할 수 있는 대기 큐를 생성하는 기본 작업을 수행함
TCP 서버와 다른 부분은 select() 함수에서 사용할 fd_set 변수형을 선언한 부분임

select() 함수를 사용하더라도 일반적인 클라이언트 통신 처리는 기본 TCP 서버와 비슷함
select() 함수를 이용해서 서버 소켓이나 클라이언트 소켓에서 읽을 데이터가 있는지 확인하고 이를 각각 처리하는 것

fd_set 자료형을 FD_ZERO() 매크로를 통해 모든 비트를 0으로 초기화하고, 서버 솤켓의 파일 디스크립터에 대한 번호를 FD_SET() 매크로를 통해 설정한다
select() 함수를 이용하여 현재의 fd_set 자료형에 대해 읽기가 가능한 이벤트가 있는지 확인하는데, 가장 마지막 인자(timeout) 가 NULL 로 설정되어 있으므로 읽기가 가능할 때까지 무한 대기

서버에 클라이언트로부터접속이 들어오면 select() 함수에서 빠져나와 fd_set 에 어떤 비트가 설정되어 있는지 확인 가능
FD_ISSET() 매크로를 사용하고, 만약 서버 소켓에서 이벤트가 발생했으면 accept() 함수를 통해 클라이언트의 접속을 받아들이고, fd_set에 FD_SET() 매크로를 통해 새로 접속한 클라이언트의 소켓에 대한 파일 디스크립터를 추가한다

서버는 다시 select() 함수로 돌아가서 서버나 클라이언트에서 데이터가 오는지 확인하고, 새로운 클라이언트의 요청이 들어오면 이전 작업을 다시 수행한다
이미 접속한 클라이언트에서 메시지가 들어오는 경우라면 다음 작업을 수행한다
for 루프와 FD_ISSET() 매크로를 이용해서 이전에 접속된 클라이언트들의 소켓의 배열에서 어떤 클라이언트로부터 데이터가 들어왔는지 확인하고, 데이터가 들어왔으면
read() 함수를 통해 데이터를 읽은 후 다시 write() 함수를 통해 클라이언트로 동일한 ㅁ네시지를 보낸다

TCP 클라이언트를 이용해서 서버에 접속할 수 있다

gani@gani:~/raspi/NetworkProgramming $ gcc -o select_server select_server.c 
gani@gani:~/raspi/NetworkProgramming $ ./select_server &
[1] 2760
gani@gani:~/raspi/NetworkProgramming $ gcc -o tcp_client tcp_client.c
gani@gani:~/raspi/NetworkProgramming $ ./tcp_client 127.0.0.1
Client is connected : 127.0.0.1
hello
Received data : hello
Received data : hello
*/