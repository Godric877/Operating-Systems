#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char* argv[])
{

int child_pid = fork();
char* argument_list[] = {(char *) NULL};
char* env[1] = { 0 };
int err = execve("./mycat",argument_list,env);
int* status;
wait(status);

return 0;

}

