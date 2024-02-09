#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
	int ret = 1;
	if (n > 1) {
		ret *= n;
		ret *= factorial(n-1);
	}
	return ret;
}

int main(int argc, char *argv[]) {
	int i = atoi(argv[1]);
	int result = factorial(i);
	printf("%d\n", result);
	return 0;
}
