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
	int fid = open(filename,O_WRONLY|O_CREAT,0700);
	printf("Parent: File opened. fd=%u\n",fid);
	int pid = fork();
	char message1[] = "hello world! I am the child\n";
	char message2[] = "hello world! I am the parent\n";
	if(pid == 0)
	{
		printf("Child %u: writing to file %u\n",getpid(),fid);
		write(fid,message1,sizeof(message1));
	}
	else
	{
		int* status;
		wait(status);
		printf("Parent %u: writing to file %u\n",getpid(),fid);
		write(fid,message2,sizeof(message2));
		printf("Parent : The child has terminated\n");
	}
return 0;
}
