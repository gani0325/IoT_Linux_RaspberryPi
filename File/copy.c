// 하나의 파일의 내용을 다음 파일로 복사하는 코드를 작성해보자
// 2개의 파일명을 입력받고 2개의 파일을 열거나 생성한 후 첫 번째 파일에서 데이터 읽어서 두 번째 파일로 쓴다
#include <unistd.h>     // 유닉스 표준 시스템 콜
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    int n, in, out;
    char buf[1024];

    // 명령형 인수로 복사할 파일명이 없는 경우에 에러 출력하고 종료
    if (argc < 3) {
        // write() : 열린 파일 디스크립터를 이용해서 파일에 데이터를 쓰며, 데이터를 쓴 만큼 파일 내의 오프셋 증가
        // 데이터의 바이터 수 반환 0 & 실패하면 -1
        // (int fd: 파일 디스크립터, const void *buf: 쓸 데이터가 있는 버퍼의 포인터형 변수, size_t cnt: 최대 바이트 수)
        write(2, "Usage : copy file1 file2\n", 25);
        return -1;
    }

    // 복사의 원본이 되는 파일을 읽기 모드로 연다
    // open() : 파일을 조작하기 위해 해당 파일 열기
    // (const char *path: 열 파일의 경로 지정, int flags: 파일을 열 때 사용할 수 있는 플래그, mode_t mode *: 파일 생성시 접근 권한)
    // O_RDONLY 읽기 O_WRONLY 쓰기 O_RDWR 읽기/쓰기
    if ((in = open(argv[1], O_RDONLY)) < 0) {
        // 1) copy.c open 해서 read 합니다.
        perror(argv[1]);
        return -1;
    }

    // 복사할 결과 파일을 쓰기 모드(새로운 파일 생성 | 기존 파일 내용 지움)로 연다
    if ((out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
        // 1) copy1.c open 해서 write 합니다.
        perror(argv[2]);
        return -1;
    }

    // 원본 파일의 내용을 읽어서 복사할 파일에 쓴다
    // read() : 열린 파일 디스크립터를 이용해서 파일로부터 데이터 읽어들임 (데이터 읽은 만큼 파일 내의 오프셋 증가) & 버퍼로 읽어들인 데이터의 바이트 수 반환
    // 데이터의 바이터 수 반환 & 파일의 끝 만나면 0 & 실패하면 -1
    // (inf fd: 읽고 싶은 파일 디스크립터, void *buf: 읽어 들인 데이터를 저장하는 버퍼의 포인터형 변수, size_t cnt: 최대 바이트 수)
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        write(out, buf, n);
    }
    // => in 으로 read()를 이용해 데이터를 읽고 out 으로 write()를 이용해 읽은 데이터를 쓴다

    // 열린 파일들 닫는다
    // close() : 열린 파일을 닫기 & 파일 닫기 성공하면 0 실패하면 -1
    // (int fd: 현재 열려 있는 파일 디스크립터)
    close(out);
    close(in);

    // 프로그램의 정상 종료 시 0을 반환
    return 0;
}

/*
gani@gani:~/raspi/File $ gcc -o copy copy.c
gani@gani:~/raspi/File $ ./copy copy.c copy1.c
gani@gani:~/raspi/File $ ls -al copy*.c
-rw-r--r-- 1 gani gani 2774  1월  2일  20:22 copy.c
-rw------- 1 gani gani 2774  1월  2일  20:23 copy1.c

copy1.c 에 copy.c 복사된 파일이 생성되어 있다
*/