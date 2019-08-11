#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc,char* argv[])
{
	int num_children = atoi(argv[1]); 
	printf("Parent is : %u\n",getpid());
	printf("Number of children is : %u\n",num_children);
	srand(time(0));

	while(num_children > 0)
	{
		int pid = fork();
		if(pid == 0)
		{
		sleep(rand()%5);
		printf("Child %u exited\n",getpid());
		exit(0);
		}
		else
		{
		printf("Child %u is created\n",pid);
		num_children--;
		}
	}

	int status = 0,wpid;
	while((wpid = wait(&status)) > 0);
	printf("Parent exited\n");

return 0;
}
