#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int pid = fork();
	if(pid == 0)
	{
	printf("Child : My process ID is %u\n",getpid());
	printf("Chlid : The parent process ID is %u\n",getppid());
	}
	else
	{
	int* status;
	int child_pid = wait(status);
	printf("Parent : My process ID is %u\n",getpid());
	printf("Parent : The child process ID is %u\n",pid);
	printf("The child with process ID %u is terminated\n",child_pid);
	}

return 0;
}

