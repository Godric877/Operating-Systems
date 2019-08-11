// time1 is sleep time for the child and time2 is the sleep time for the grandchild
// Edit them accordingly to observer the required results
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void childSignalHandler(int signo) {
    int status;

    int pid = wait(&status);
	
    printf("Parent %d: Child %d is exited\n",getpid(),pid);
}

int main()
{
        int time1 = 4;
	int time2 = 9;
	int pid = fork();

	if(pid == 0)
	{
	printf("Child %d is created by parent %d (sleeps for %d seconds)\n",getpid(),getppid(),time1);
	int pid1 = fork();
	 if(pid1 == 0)
	 {
         printf("Child %d is created by parent %d (sleeps for %d seconds)\n",getpid(),getppid(),time2);
	 sleep(time2);
	 exit(0);
	 }
	 else
	 {
         signal(SIGCHLD,childSignalHandler);
	 sleep(time1);
	 sleep(1);
         exit(0);
	 }
	}

	signal(SIGCHLD,childSignalHandler);
	for(int i=0;i<2;i++) sleep(10);
return 0;
}

