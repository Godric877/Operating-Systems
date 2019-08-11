#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
	if(argc <= 1)
	{
		exit();
	}

	char *prog[argc-1];
	for(int i=1;i<argc;i++)
	{
		prog[i-1] = argv[i];
	}

	int pid = fork();

	if(pid == 0)
	{
		exec(prog[0],prog);
		exit();
	}
	else
	{
		wait();
		exit();
	}

}