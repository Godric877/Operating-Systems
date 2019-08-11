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
	int loop = num_children;
	int child_pid[num_children];
	srand(time(0));

	while(num_children > 0)
	{
		int pid = fork();
		if(pid == 0)
		{
		sleep(rand()%5);
		exit(0);
		}
		else
		{
		printf("Child %u is created\n",pid);
		child_pid[num_children-1] = pid;
		num_children--;
		}
	}

	int status = 0,wpid;
	int i = loop;
	while((wpid = waitpid(child_pid[loop-i],&status,0)) > 0)
	{
	printf("Child %u exited\n",child_pid[loop-i]);
	i--;
	}
	printf("Parent exited\n");

return 0;
}
