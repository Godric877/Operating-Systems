#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_SZ 1000

void do_some_job(int starts)
{
	int pid;
	int ends;

	pid = getpid();

	printf(1, "\nchild %d: starts executiion at %d \n", pid, starts );

	sleep(10);
	
	ends = uptime();
	printf(1, "child %d: ends at %d \n", pid, ends);
	printf(1, "child %d: time taken = %d ticks \n", pid, (ends - starts) );

}

int main(int argc, const char **argv) 
{
	int pid1;
	int starts;

	printf(1, "\nparent %d: Creating first child \n", getpid());
	starts = uptime();
	pid1 = fork();
	if(pid1 == 0) 
	{ 
		do_some_job(starts);
		exit();
	}

	wait();

	// Sending pause signal to terminated child

	int r1 = pause(pid1, 700);
	printf(1,"Current Pause Time %d\n",700);
	if(r1 == 0) printf(1,"Pause successful\n");
	else printf(1,"Pause unsuccessful\n");

	exit();

}
