#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum)
{
	if(signum == 2) 
	{
		printf("Interrupt SIGINT received.\n");
	}
	if(signum == 15)
	{
		printf("Interrupt SIGTERM received.\n");
	}
	
    printf("Do you really want to quit (y|n)?\n");
    char inp[5];
    scanf("%s",inp);
    if(inp[0] == 'y') exit(0);
    else return;
}

int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    while(1)
    {
    sleep(10);
	}
    return 0;
}
