// 소켓을 이용해서 UDP로 서로 통신하기
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define UDP_PORT 5100

int main(int argc, char **argv)
{
    int sockfd,n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char mesg[1000];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 	/* UDP를 위한 소켓 생성 */

    /* 접속되는 클라이언트를 위한 주소 설정 후 운영체제에 서비스 등록 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(UDP_PORT);
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    /* 클라이언트로부터 메시지를 받아서 다시 클라이언트로 전송 */
    do {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &len);
        sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        mesg[n] = '\0';
        printf("Received data : %s\n", mesg);
    } while(strncmp(mesg, "q", 1));

    close(sockfd); 				/* 사용이 끝난 후 소켓 닫기 */

    return 0;
}

/*
서버는 클라이언트로부터 데이터를 받아서 그대로 다시 되돌려주는 에코 (ECHO) 서버

socket() 함수에 SOCK_DGRAM 옵션을 사용해서 UDP를 위한 소켓의 파일 디스크립터를 연다
해당 프로그램을 서버로 사용하기 위해서는 운영체제에 서비스를 등록해야 한다 -> bind()
bind() 함수에 sockaddr_in 구조체를 사용해서 서버의 주소와 포트 번호를 설정한다
설정하기 전에 sockaddr_in 구조체를 memset() 함수를 통해 0으로 초기화
서버의 주소가 동적으로 바귀더라도 사용하기 쉽도록 INADDR_ANY 사용
htonl() 와 htons() 함수를 이용해서 IP 주소와 포트 번호를 네트워크 순서로 엔디안 변경

루프문 돌면서 클라이언트로부터 데이터를 받고, 받은 데이터를 sendto() 함수를 이용해서 다시 클라이언트로 전송
클라이언트에서 문자열 'q' 가 오면 종료, 모든 작업이 끝나면 close() 함수로 소켓 닫기
*/