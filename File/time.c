#include <stdio.h>
#include <time.h>
#include <sys/time.h>       // gettimeofday() 함수
#include <stdlib.h>

int main(int argc, char **argv) {
    int i, j;
    time_t rawtime;
    struct tm *tm;
    char buf[BUFSIZ];
    struct timeval mytime;

    // 현재의 시간 구하기
    time(&rawtime);
    // 현재의 시간을 화면에 출력
    printf("time : %u\n", (unsigned)rawtime);

    // 현재의 시간 구하기
    gettimeofday(&mytime, NULL);
    printf("gettimeofday : %ld/%d\n", mytime.tv_sec, mytime.tv_usec);

    // 현재의 시간을 문자열로 바꿔서 출력
    printf("ctime : %s", ctime(&rawtime));

    // 환경 변수를 설정한다
    putenv("TZ=PST3PDT");
    // TZ 변수 설정한다
    tzset();
    tm = localtime(&rawtime);
    // 현재의 시간을 tm 구조체를 이용해서 출력
    printf("asctime : %s", asctime(tm));

    // 사용자 정의 문자열 저장
    strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Y", tm);
    printf("strftime : %s\n", buf);

    return 0;
}

/*
time() : 시간 출력을 위해 먼저 현재 시간을 가져온다 (이해하기 힘든 큰 숫자로 나온다)
ctime(), asctime() : 사람이 이해할 수 있는 형태로 변환한다

시간과 관련된 구조체는 tm 구조체
gmtime() : UTC 시간을 반환
localtime() : 현재 로컬에 맞는 시간 사용
strftime() : 원하는 서식에 맞도록 출력

현재 출력되는 시간대를 바꾸고 싶다면 환경 변수 조정
putenv() : 현재 환경을 설정
"TZ" : 시간대와 관련된 환경 변수
tzset() : 변경된 환경 변수
setlocale() : 로케일 변경

gani@gani:~/raspi/File $ gcc -o time time.c
gani@gani:~/raspi/File $ ./time
*/