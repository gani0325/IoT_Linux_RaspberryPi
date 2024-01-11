// FIFO 사용하기 위한 Client
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFOFILE "fifo"

int main(int argc, char **argv)
{
    int n, fd;
    char buf[BUFSIZ];

    if((fd = open(FIFOFILE, O_WRONLY)) < 0) { 		/* FIFO를 연다. */
        perror("open()");
        return -1;
    }

    while ((n = read(0, buf, sizeof(buf))) > 0) 	/* 키보드로부터 데이터를 입력받는다 */
        write(fd, buf, n); 				/* FIFO로 데이터를 보낸다. */

    close(fd);

    return 0;
}

/*
FIFO 파일을 연 후 키보드로부터 데이터를 읽어서 서버에게로 해당 데이터를 출력한다

@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ gcc -o fifo_client fifo_client.c 
@gani0325 ➜ /workspaces/IoT_Linux_RaspberryPi/Process_Signal (main) $ ./fifo_client
*/