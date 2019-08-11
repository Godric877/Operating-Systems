#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void childSignalHandler(int signo) {
    int status;

    int pid = wait(&status);
	
    if(pid == 0)
    {
	return;
    }
    else
    {
    printf("Child %d is exited\n",pid);
    }
}

int main(int argc,char* argv[])
{
	int num_children = atoi(argv[1]);
	int n = num_children; 
	srand(time(0));
	int max = 0;

	while(num_children > 0)
	{
		int pid = fork();
                int t = rand()%10 + 1;
		max = (max > t) ? max : t;
		if(pid == 0)
		{
		sleep(t);
		//printf("Child %u exited\n",getpid());
		exit(0);
		}
		else
		{
		printf("Child %u is created (sleeps for %d seconds)\n",pid,t);
		num_children--;
		}
	}
	
	signal(SIGCHLD,childSignalHandler);
	for(int i=0;i<n;i++) sleep(max+1);

return 0;
}
