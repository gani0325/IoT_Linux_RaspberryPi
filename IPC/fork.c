// fork.c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

char ppid_str[32];
char *const ps_argv[] = {
	"/bin/ps",
	"-fx",
	NULL,
};

int run_ps()
{
	int status;
	pid_t pid;
	printf("Process list > \n");

	pid = fork();
	if (pid == 0)
	{
		execve(ps_argv[0], ps_argv, environ);
	}
	waitpid(pid, &status, 0 /*no option */);
}

int main()
{
	int status;
	pid_t pid;
	pid = fork();

	if (pid == 0)
	{
		printf("[Child] pid - %d ppid - %d \n", getpid(), getppid());
		sleep(1);
		printf("[Child] exit\n");
		return 3;
	}
	else
	{
		printf("[Parent] pid - %d\n", getpid());
		run_ps();
		sleep(2);
		run_ps();

		waitpid(pid, &status, 0 /* option */);
		printf("[Parent] child exit : %d\n", WEXITSTATUS(status));

		run_ps();
		printf("[Parent] exit\n");
	}
	return 0;
}