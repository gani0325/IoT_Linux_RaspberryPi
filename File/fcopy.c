// 표준 입출력 함수를 이용해서 파일을 복사한다
// 출력 방향을 정하는 함수인 fprint() 함수와 표준 에러(etreer) 이용해서 저장한다
#include <stdio.h>

int main(int argc, char **argv) {
    int n;
    FILE *in, *out;
    char buf[BUFSIZ];

    // 명령형 인수로 복사할 파일명이 없는 경우에 에러를 출력하고 종료한다
    if (argc != 3) {
        fprintf(stderr, "Usage: fcopy file1 file2\n");
        return -1;
    }

    // 복사의 원본이 되는 파일을 읽기 모드로 연다
    // fopen() : 지정한 경로에 있는 파일의 스트림을 열고 버퍼를 할당한다
    // (const char *path: 파일의 경로, const char *mode: 열 스트림의 형태를 명시)
    // 파일 열기 성공하면 FILE 포인터 반환 & 실패하면 NULL (errno 에 에러 정보)
    if ((in = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        return -1;
    }

    // 복사할 결과 파일을 쓰기 모드 (새로운 파일 생성 | 기존에 파일 내용 지움)로 연다
    if ((out = fopen(argv[2], "w")) == NULL) {
        perror(argv[2]);
        return -1;
    }

    // 원본 파일에서 파일 내용을 읽어서 복사할 파일에 쓴다
    // fread() : 지정된 스트림에서 원하는 크기만큼의 데이터를 읽고 쓸 수 있는 버퍼 기반 입출력 함수
    // (void *ptr: 입력이나 출력을 위한 버퍼 공간, size_t size: 입출력 데이터 하나의 크기, size_t nmemb: 입출력을 위한 데이터 수, FILE *fp: FILE 스트림)
    while ((n = fread(buf, sizeof(char), BUFSIZ, in)) > 0) {
        fwrite(buf, sizeof(char), n, out);
    }
    // => 입출력을 위한 두 개의 스트림을 fopen() 함수를 이용해 각각 연다
    // 읽을 파일은 r옵션 사용하여 읽기 모드로 열고, 쓸 파일은 w옵션 사용하여 쓰기 모드로 열었다
    // fread() 로 첫 번째 스트림으로부터 데이터를 읽고, fwrite() 로 두 번째 스트림에 읽은 데이터를 쓴다
    // 복사가 완료되면 fclose() 이용해서 두 개의 스트림을 모두 닫는다

    // 열린 파일을 닫는다
    // fclose() : FILE 포인터가 가리키는 스트림과 파일을 분리한 후, 파일을 닫고 자원을 반환한다
    // 출력 버퍼에 있는 데이터는 스트림에 출력(저장)하고 입력 버퍼에 있는 데이터를 버린 후에 버퍼를 해제한다
    // (FILE *fp: FILE 스트림을 사용)
    // 함수의 호출에 성공하면 0 반환 & 실해파면 EOF(01) 반환
    fclose(out);
    fclose(in);

    return 0;
}