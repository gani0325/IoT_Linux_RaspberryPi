// 현재 파일의 권한을 변경하는 코드
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    struct stat statbuf;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        return -1;
    }

    // 1) 파일에 존재 여부에 대한 정보를 가져온다
    if (stat(argv[1], &statbuf) < 0) {
        perror("stat");
        return -1;
    }

    // 2) chmod 함수를 호출해서 파일에 set-grout-ID 를 설정하고 그룹의 실행 권한을 해제한다
    if (chmod(argv[1], (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        perror("chmod");
        return -1;
    }

    // 3) 8진수 이용해서 파일의 권한을 644("rw-r--r--") 절댓값으로 설정한다
    if (chmod(argv[2], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
        perror("chmod");
        return -1;
    }
    // => 빌드해서 실행하기 전 4.txt 실행 권한을 주고 실행하기

    return 0;
}

/*
첫 번째 파일은 그룹의 set-grout-ID 가 설정되고 실행 권한이 제거됨
두 번째 파일은 644 권한으로 설정되어 있음
*/