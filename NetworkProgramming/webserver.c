// webserver.c
// HTTP 프로토콜과 스레드, 소켓 프로그래밍을 이용해서 웹 브라우저와 통신할 수 있는 웹 서버
// 웹 서버에 스레드를 이용하면 여러 웹 브라우저에서 동시에 접근할 수 있다
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* 스레드 처리를 위한 함수 */
static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);

int main(int argc, char **argv)
{
    int ssock;
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    /* 프로그램을 시작할 때 서버를 위한 포트 번호를 입력받는다. */
    if(argc!=2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    /* 서버를 위한 소켓을 생성한다. */
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if(ssock == -1) {
        perror("socket()");
        return -1;
    }

    /* 입력받는 포트 번호를 이용해서 서비스를 운영체제에 등록한다. */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = (argc != 2)?htons(8000):htons(atoi(argv[1]));
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1) {
        perror("bind()");
        return -1;
    }
    
    /* 최대 10대의 클라이언트의 동시 접속을 처리할 수 있도록 큐를 생성한다. */
    if(listen(ssock, 10) == -1) {
        perror("listen()");
        return -1;
    }

    while(1) {
        char mesg[BUFSIZ];
        int csock;

        /* 클라이언트의 요청을 기다린다. */
        len = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr*)&cliaddr, &len);

        /* 네트워크 주소를 문자열로 변경 */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client IP : %s:%d\n", mesg, ntohs(cliaddr.sin_port));

        /* 클라이언트의 요청이 들어오면 스레드를 생성하고 클라이언트의 요청을 처리한다. */
        pthread_create(&thread, NULL, clnt_connection, &csock);
        pthread_join(thread, NULL);
    }
    return 0;
}
    
void *clnt_connection(void *arg)
{
    /* 스레드를 통해서 넘어온 arg를 int 형의 파일 디스크립터로 변환한다. */
    int csock = *((int*)arg);
    FILE *clnt_read, *clnt_write;
    char reg_line[BUFSIZ], reg_buf[BUFSIZ];
    char method[BUFSIZ], type[BUFSIZ];
    char filename[BUFSIZ], *ret;

    /* 파일 디스크립터를 FILE 스트림으로 변환한다. */
    clnt_read = fdopen(csock, "r");
    clnt_write = fdopen(dup(csock), "w");

    /* 한 줄의 문자열을 읽어서 reg_line 변수에 저장한다. */
    fgets(reg_line, BUFSIZ, clnt_read);
    
    /* reg_line 변수에 문자열을 화면에 출력한다. */
    fputs(reg_line, stdout);

    /* ' ' 문자로 reg_line을 구분해서 요청 라인의 내용(메소드)를 분리한다. */
    ret = strtok(reg_line, "/ ");
    strcpy(method, (ret != NULL)?ret:"");
    if(strcmp(method, "POST") == 0) { 		/* POST 메소드일 경우를 처리한다. */
        sendOk(clnt_write); 			/* 단순히 OK 메시지를 클라이언트로 보낸다. */
        goto END;
    } else if(strcmp(method, "GET") != 0) {	/* GET 메소드가 아닐 경우를 처리한다. */
        sendError(clnt_write); 			/* 에러 메시지를 클라이언트로 보낸다. */
        goto END;
    }

    ret = strtok(NULL, " "); 			/* 요청 라인에서 경로(path)를 가져온다. */
    strcpy(filename, (ret != NULL)?ret:"");
    if(filename[0] == '/') { 			/* 경로가 '/'로 시작될 경우 /를 제거한다. */
        for(int i = 0, j = 0; i < BUFSIZ; i++, j++) {
            if(filename[0] == '/') j++;
            filename[i] = filename[j];
            if(filename[j] == '\0') break;
        }
    }

    /* 메시지 헤더를 읽어서 화면에 출력하고 나머지는 무시한다. */
    do {
        fgets(reg_line, BUFSIZ, clnt_read);
        fputs(reg_line, stdout);
        strcpy(reg_buf, reg_line);
        char* buf = strchr(reg_buf, ':');
    } while(strncmp(reg_line, "\r\n", 2)); 	/* 요청 헤더는 ‘\r\n’으로 끝난다. */

    /* 파일의 이름을 이용해서 클라이언트로 파일의 내용을 보낸다. */
    sendData(clnt_write, type, filename);

END:
    fclose(clnt_read); 				/* 파일의 스트림을 닫는다. */
    fclose(clnt_write);
    pthread_exit(0); 				/* 스레드를 종료시킨다. */

    return (void*)NULL;
}
    
int sendData(FILE* fp, char *ct, char *filename)
{
    /* 클라이언트로 보낼 성공에 대한 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server:Netscape-Enterprise/6.0\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n";
    char end[ ] = "\r\n"; 			/* 응답 헤더의 끝은 항상 \r\n */
    char buf[BUFSIZ];
    int fd, len;

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);
    fputs(end, fp);

    fd = open(filename, O_RDWR); 		/* 파일을 연다. */
    do {
        len = read(fd, buf, BUFSIZ); 		/* 파일을 읽어서 클라이언트로 보낸다. */
        fputs(buf, fp);
    } while(len == BUFSIZ);

    close(fd); 					/* 파일을 닫는다. */

    return 0;
}
    
void sendOk(FILE* fp)
{
    /* 클라이언트에 보낼 성공에 대한 HTTP 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n\r\n";

    fputs(protocol, fp);
    fputs(server, fp);
    fflush(fp);
}
    
void sendError(FILE* fp)
{
    /* 클라이언트로 보낼 실패에 대한 HTTP 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 400 Bad Request\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n";
    char cnt_len[ ] = "Content-Length:1024\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n\r\n";

    /* 화면에 표시될 HTML의 내용 */
    char content1[ ] = "<html><head><title>BAD Connection</title></head>";
    char content2[ ] = "<body><font size=+5>Bad Request</font></body></html>";
    printf("send_error\n");

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fputs(content1, fp);
    fputs(content2, fp);
    fflush(fp);
}

/*
메인 함수 : 서버를 초기화하고 클라이언트 요청이 들어오면 스레드를 생성
스레드 처리 함수에서는 클라이언트에서 메시지를 읽고 HTTP 헤더를 분석하여 해당 내용을 처리한다

클라이언트에서 데이터를 요청하면 sendData() 함수를 호출하고 파일을 읽어서 해당 데이터를 클라이언트로 보낸다
sendOK() 함수는 요처어된 내용에 문제가 없고 단순 요청인 경우 이를 처리하여 상태 메시지를 보낸다
sendError() 함수는 요청된 내용에 문제가 있는 경우 에러 응답 메시지를 보낸다

main() 함수에서는 웹 서비스를 사용하기 위해 소켓을 생성하고 운영체제에 서비스를 등록한다
서버의 주소에 INADDR_ANY 값을 사용하여 간단히 서버 주소를 처리할 수 있도록 하고, 입력받은 포트 번호를 이용한다
최대 10대의 클라이언트의 동시 접속을 처리할 수 있도록 대기 큐를 생성한 후 클라이언트를 기다린다

클라이언트의 요청이 들어오면 접속된 클라이언트의 정보를 출력하고 새로운 스레드르 생성한다
스레드를 생성할 때 새로 들어온 클라이언트의 소켓을 스레드의 인자로 넘겨, 스레드 처리 함수에서 해당 클라이언트와 통신할 수 있도록 함

clnt_connection() 함수는 클라이언트의 접속을 처리하는 함수
클라이언트 소켓을 스레드의 인자로 넘겨받아서 int 형의 파일 디스크립터로 변환한 후, 다시 fdopen() 함수를 통해 파일 스트림으로 변환한다
HTTP의 헤더들은 줄 단위로 처리되는데, 파일 스트림을 줄 단위로 처리하는 fgets() 함수로 사용하면 편리하다

먼저 HTTP 요청 라인을 읽고 분석한다. 요청 라인은 메소드, 경로, HTTP 버전으로 구성됨
문자열은 strtok() 함수를 이용해서 분석할 수 있는데 공백을 인자로 사용해서 분리한다

요청 라인의 첫 번재 요소는 메소드, POST 메소드에 대해서는 간단하게 확인 응답만 클라이언트로 보내고, GET 메소드에 대해서는 요청한 경로의 파일을
읽어서 클라이언트에게 보내준다
현재의 웹 서버는 GET, POST 메소드만 처리하는데, 클라이언트이 두 메소드가 아닌 경우에는 에러 메시지를 클라이언트로 보낸다

요청 라인 뒤에는 메시지 헤더가 오는데 앞의 웹 서버에서는 이를 처리하지 않으므로 C언어의 순환문을 통해서 전부 무시한다
메시지 본문으로 오는 문서의 크기를 알고 싶은 경우, Content-Length 항목을 사용하면 되는데, 관련 사항은 순환문 안에 구현하면 된다
요청 헤더는 '\r\n' 으로 끝나고 헤더와 메시지 본문을 구분한다
클라이언트에 대한 처리가 끝나면 스트림을 닫고 스레드를 종료한다
sendData() 함수는 요청한 파일을 열어 내용을 읽고 클라이언트에게 보낸다. 파일 내용을 보내기 전에 HTTP 에 대한 응답 헤더를 보내며
그 뒤에 파일 내용을 함께 보낸다. 파일의 사요이 끝나면 열엿던 파일을 닫고 함수를 종료한다

-lpthread 옵션을 줘서 스레드 라이브러리를 별도로 링크해야 한다
웹 서버 실행 시, 웹 서비스에서 사용할 포트 번호를 지정한다

index.html 과 함께 실행한다

gani@gani:~/raspi/NetworkProgramming $ gcc -o webserver webserver.c -lpthread
gani@gani:~/raspi/NetworkProgramming $ ./webserver 8080
*/