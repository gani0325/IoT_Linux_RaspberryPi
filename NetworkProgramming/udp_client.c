// udp_client.c
// 소켓을 이용해서 UDP로 서로 통신하기
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define UDP_PORT 5100

int main(int argc, char **argv)
{
    int sockfd, n;
    socklen_t clisize;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];

    if(argc != 2) {
        printf("usage : %s <IP address>\n", argv[0]);
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 	/* UDP를 위한 소켓 생성 */

    /* 서버의 주소와 포트 번호를 이용해서 주소 설정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    /* 문자열을 네트워크 주소로 변경 */
    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(UDP_PORT);

    /* 키보드로부터 문자열을 입력받아 서버로 전송 */
    do {
        fgets(mesg, BUFSIZ, stdin);
        sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr *)&servaddr,
        sizeof(servaddr));
        clisize = sizeof(cliaddr);

        /* 서버로부터 데이터를 받아서 화면에 출력 */
        n = recvfrom(sockfd, mesg, BUFSIZ, 0, (struct sockaddr*) &cliaddr, &clisize);
        mesg[n] = '\0';
        fputs(mesg, stdout);
    } while(strncmp(mesg, "q", 1));

    close(sockfd);

    return 0;
}

/*
프로그램을 싫랭할 때 접속할 서버의 주소를 입력하면 되는데
입력받은 문자는 inet_pton() 함수를 이용해서 16진수 주소로 변환할 수 있다

라즈베리파이로 접속중이라면 서버를 백그라운드 모드로 실행한다 &
같은 라즈베리파이 안에서 UDP 서버와 클라이언트를 함께 실행할 때 서버 주소로 127.0.0.1
127.0.0.1은 루프백 주소라고도 하는데, 현재 시스템 자체의 IP 주소를 지칭할 때 사용한다

클라이언트에서 문자열을 입력하면 UDP를 통해 클라이언트와 서버 간 통신이 이뤄진다
서버는 운영체제에서 넘어온 데이터를 받아서 화면에 출력한 뒤 바로 출력하고
다시 클라이언트의 입력을 기다린다
*/