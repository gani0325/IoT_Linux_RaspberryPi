// kbhit() 함수는 키보드의 입력을 감지하는 함수 (유닉스에서 fcntl() 함수와 터미널 제어로 구현 가능)
// 터미널은 로컬이나 네트워크를 통해서 원격으로 연결되어 유닉스 시스템과의 대화를 위해 사용되는 디바이스로 /dev/tty 디바이스 파일 사용함
// 터미널의 기본 제어는 termios 구조체와 관련 함수들을 사용함
// (화면에 출력할 수 없도록 터미널 제어 함수인 tcgetattr, tcsetattr 이용해서 터미널에 대한 값을 가져오거나 설정함)
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int kbhit(void) {
    // 터미널에 대한 구조체
    struct termios oldt, newt;
    int ch, oldf;
    
    // 현재 터미널에 설정된 정보 가져옴
    tcgetattr(0, &oldt);
    newt = oldt;
    // 정규 모드 입력과 에코를 해제 (ICANON 정규모드로 입력이 이뤄지게 함, ECHO 는 키보드로 입력한 내용을 모니터로 출력하도록 함)
    newt.c_lflag &= ~(ICANON | ECHO);
    // 새로운 값으로 터미널 설정 (TCSANOW 바로 터미널에 변경된 속성이 적용되도록 함)
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_GETFL, 0);
    // 입력을 논블로킹 모드로 설정
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);
    // getchar() : 키보드로 입력된 문자가 화면에 자동으로 출력됨
    ch = getchar();

    if (ch != EOF) {
        // 앞에서 읽으며 꺼낸 문자를 다시 넣는다
        ungetc(ch, stdin);
    }
    return 0;
}

int main(int argc, char **argv) {
    for (int i = 0;; i++) {
        if (kbhit()) {
            switch(getchar()) {
                case 'q':
                    goto END;
                    break;
            };
        }
        // 현재 카운트한 숫자를 출력
        printf("%20d\t\t\r", i);
        // 100밀리초 동안 잠시 쉰다
        usleep(1000);
    }
END:
    printf("Good Bye!\n");
    return 0;
}

// => 숫자가 제자리에서 증가한다
// 한 줄에서만 숫자의 증가를 표시하기 위해 "\r" 사용함

/*
gani@gani:~/raspi/File $ gcc -o hitkey hitkey.c
gani@gani:~/raspi/File $ ./hitkey
                8580
*/