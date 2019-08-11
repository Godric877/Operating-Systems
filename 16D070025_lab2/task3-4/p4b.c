#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,char* argv[])
{
	char* filename = argv[1];
	int cl_id = close(0);
	int fid = open(filename,O_RDONLY,0700);

	int pid = fork();

	if(pid == 0)
	{
		char* env[1] = { 0 };
		char* arguement_list[] = {(char *)NULL};
		int err = execve("./mycat",arguement_list,env);
	}
	else
	{
		printf("Parent: File opened: fd=%u\n",fid);
		int* status;
		wait(status);
		printf("\nParent : The child has terminated.\n");
	}

return 0;
}
