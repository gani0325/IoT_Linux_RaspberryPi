a.out : distance.o main.o
	ld -o a.out distance.o main.o \
	-lm -lc -dynamic-linker=/lib64/ld-linux-x86-64.so.2 \
	/usr/lib/x86_64-linux-gnu/crt1.o

distance.o : distance.c
	gcc -c -o distance.o distance.c

main.o : main.c
	gcc -c -o main.o main.c