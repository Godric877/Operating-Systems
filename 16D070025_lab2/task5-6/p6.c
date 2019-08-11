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
	getchar();
	}
	else
	{
	printf("Parent : My process ID is %u\n",getpid());
	printf("Parent : The child process ID is %u\n",pid);
	sleep(60);
	int* status;
        wait(status);
	printf("Parent : The parent has terminated.\n");
	}

return 0;
}

