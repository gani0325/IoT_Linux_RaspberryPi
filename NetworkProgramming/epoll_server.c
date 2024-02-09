// epoll_server.c
// TCP 서버 코드를 epoll 의 함수를 이용하여 병렬 처리할 수 있도록 변경하기
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SERVER_PORT 5100 			/* 서버의 포트 번호 */
#define MAX_EVENT 32

/* 파일 디스크립터를 넌블로킹 모드로 설정 */
void setnonblocking(int fd)
{
    int opts = fcntl(fd, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
}

int main(int argc, char **argv)
{
    int ssock, csock;
    socklen_t clen;
    int n, epfd, nfds = 1; 			/* 기본 서버 추가 */
    struct sockaddr_in servaddr, cliaddr;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENT];
    char mesg[BUFSIZ];

    /* 서버 소켓 디스크립터를 연다. */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    setnonblocking(ssock); 			/* 서버를 넌블로킹 모드로 설정 */

    memset(&servaddr, 0, sizeof(servaddr)); 	/* 운영체제에 서비스 등록 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
    if(bind(ssock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    if(listen(ssock, 8) < 0) { 			/* 클라이언트의 소켓들을 위한 큐 생성 */
        perror("listen()");
        return -1;
    }

    /* epoll_create() 함수를 이용해서 커널에 등록 */
    epfd = epoll_create(MAX_EVENT);
    if(epfd == -1) {
        perror("epoll_create()");
        return 1;
    }

    /* epoll_ctl() 함수를 통해 감시하고 싶은 서버 소켓을 등록 */
    ev.events = EPOLLIN;
    ev.data.fd = ssock;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &ev) == -1) {
        perror("epoll_ctl()");
        return 1;
    }

    do {
        epoll_wait(epfd, events, MAX_EVENT, 500);
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == ssock) { 	/* 읽기가 가능한 소켓이 서버 소켓인 경우 */
                clen = sizeof(struct sockaddr_in);

                /* 클라이언트의 요청 받아들이기 */
                csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
                if(csock > 0) {
                    /* 네트워크 주소를 문자열로 변경 */
                    inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                    printf("Client is connected : %s\n", mesg);
                    setnonblocking(csock); 	/* 클라이언트를 넌블로킹 모드로 설정 */

                    /* 새로 접속한 클라이언트의 소켓 번호를 fd_set에 추가 */
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = csock;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
                    nfds++;
                    continue;
                }
            } else if(events[i].events & EPOLLIN) { 	/* 클라이언트의 입력 */
                if(events[i].data.fd < 0) continue; 	/* 소켓이 아닌 경우의 처리 */
                memset(mesg, 0, sizeof(mesg));

                /* 해당 클라이언트에서 메시지를 읽고 다시 전송(Echo) */
                if((n = read(events[i].data.fd, mesg, sizeof(mesg))) > 0) {
                    printf("Received data : %s", mesg);
                    write(events[i].data.fd, mesg, n);
                    close(events[i].data.fd); 		/* 클라이언트 소켓을 닫고 지운다. */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    nfds--;
                }
            }
        }
    } while(strncmp(mesg, "q", 1));

    close(ssock); 				/* 서버 소켓을 닫는다. */

    return 0;
}

/*
TCP 서버와 같이 소켓을 생성한 후 bind()와 listen() 함수를 이용해서 서비스를 운영체제에 등록한다
epoll_create() 함수를 이용해서 감시하고 싶은 파일 디스크립터의 숫자를 넣어줬는데, 서버 + 클라이언트의 수로 설정하면 된다
epoll_ctl() 함수에 서버 소켓을 추가하였다. 서버 소켓은 클라이언트의 접속을 감시할 것이므로 입력 (EPOLLIN) 으로 이벤트 설정

서버에 클라이언트가 접속되면 epoll_wait() 함수에서 넘어가게 되는데, 이벤트가 발생한 소켓이 서버이면 accpet() 함수를 통해서 받아들이고,
epoll_ctl() 함수를 통해 다시 커널에 클라이언트의 소켓 디스크립터를 감시할 수 있도록 추가한다

클라이언트 소켓으로부터 이벤트가 발생하면 클라이언트에서 온 데이터를 읽어서 다시 클라이언트로 전송(에코) 하고 클라이언트한테 소켓을 닫은 후
epoll_ctl() 함수를 통해 커널에 추가한 클라이언트 소켓의 감시를 삭제한다

클라이언트는 tcl_client 로 사용
epoll 이 select 에 비해 속도는 빠르지만, 특정 플랫폼에서는 사용할 수 없다

gani@gani:~/raspi/NetworkProgramming $ gcc -o epoll_server epoll_server.c 
gani@gani:~/raspi/NetworkProgramming $ ./epoll_server &
gani@gani:~/raspi/NetworkProgramming $ gcc -o tcp_client tcp_client.c 
gani@gani:~/raspi/NetworkProgramming $ ./tcp_client 
gani@gani:~/raspi/NetworkProgramming $ ./tcp_client 127.0.0.1
*/