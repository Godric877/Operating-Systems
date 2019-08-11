#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void recursive_fork(int depth)
{
	if(depth == -1) return;
	int pid = fork();
	if(pid == 0)
	{
	recursive_fork(depth-1);
	}
	else
	{
	printf("Child %u is created\n",getpid());
	int* status;
	wait(status);
	printf("Child %u exited\n",getpid());
	}
}

int main(int argc,char* argv[])
{
	int num_children = atoi(argv[1]); 
	printf("Parent is : %u\n",getpid());
	printf("Number of children is : %u\n",num_children);
	int pid = fork();
	if(pid == 0)
	{
	recursive_fork(num_children-1);
	}
	else
	{
	int* status;
	wait(status);
	printf("Parent exited\n");
	}
return 0;
}
