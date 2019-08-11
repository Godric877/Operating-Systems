#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_SZ 1000000

// Issue continue signal after sending kill signal and waiting

int main(int argc, const char **argv) 
{
	int ret = fork();

	if(ret == 0) 
	{ 
		printf(1,"Child %d created\n",getpid());
		sleep(100);
	}
	else
	{
		int r1 = signal_process(ret,SIGNAL_KILL);
		if(r1 == 0) printf(1,"parent: sending kill signal to child\n");
		else printf(1,"parent: kill signal failed\n");

		sleep(10);

		int pid = wait();
		if(pid > 0) printf(1,"Child process with pid %d terminated\n",pid);
		else printf(1,"Wait Error\n");

		int r2 = signal_process(ret,SIGNAL_CONTINUE);
		if(r2 == 0) printf(1,"parent: sending continue signal to child\n");
		else printf(1,"parent: continue signal failed\n");
	}

	exit();
}