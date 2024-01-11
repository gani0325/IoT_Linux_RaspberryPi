// FIFO 사용하기 위한 Server
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 			/* read(), close(), unlink() 등의 시스템 콜을 위한 헤더 파일 */
#include <sys/stat.h>

#define FIFOFILE "fifo"

int main(int argc, char **argv)
{
    int n, fd;
    char buf[BUFSIZ];
    unlink(FIFOFILE); 			/* 기존의 FIFO 파일을 삭제한다. */

    if(mkfifo(FIFOFILE, 0666) < 0) { 	/* 새로운 FIFO 파일을 생성한다. */
        perror("mkfifo()");
        return -1;
    }

    if((fd = open(FIFOFILE, O_RDONLY)) < 0) { 		/* FIFO를 연다. */
        perror("open()");
        return -1;
    }

    while((n = read(fd, buf, sizeof(buf))) > 0) 	/* FIFO로부터 데이터를 받아온다. */
        printf("%s", buf); 		/* 읽어온 데이터를 화면에 출력한다. */

    close(fd);

    return 0;
}

/*
FIFO 서버는 FIFO 통신을 위한 파일을 생성하고 FIFO 파일을 연 후, 클라이언트에서 데이터가 오면 읽어서 화면에 출력한다
FIFO 서버는 백그라운드 모드로 실행한 후 FIFO 클라이언트를 실행한다
FIFO 클라이언트에서 입력해서 보낸 내용이 서버를 통해서 화면에 표시된다
Ctrl+C 눌러서 클라이언트 실행을 종료하면 FIFO 가 깨지기 때문에 서버도 종료된다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o fifo_server fifo_server.c 
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./fifo_server &
*/