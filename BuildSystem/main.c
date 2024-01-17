// main.c
#include <stdio.h>
#include "distance.h"

int main() {
	double ret = distance(0, -1, 4, 0);
	printf("return value : %lf\n", ret);
	return 0;
}	

/*
make

gcc -c -o distance.o distance.c
gcc -c -o main.o main.c
ld -o a.out distance.o main.o -lm -lc -dynamic-linker=/lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o
*/