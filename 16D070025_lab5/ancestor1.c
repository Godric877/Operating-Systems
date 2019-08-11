#include "types.h"
#include "stat.h"
#include "user.h"

int main() 
{
	int p1 = getppid();
	int p2 = -1;
	while(p1 != p2)
	{
		printf(1,"Sending kill signal to parent %d\n",p1);
		if( signal_process(p1,SIGNAL_KILL) > 0 ) 
		{
			p2 = p1;
			p1 = getppid();
		}
		else
		{
			printf(1,"Kill signal failed\n");
			break;
		}
	}
	
	exit();
}