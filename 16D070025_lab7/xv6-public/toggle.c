#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int ret;

	init_counters();
	int i = 0,j = 0;

	ret = fork();

	if(ret == 0)
	{
		while(i < 30)
		{
			if(get_var(0) == 0) continue;
			acquire_lock(1);
			set_var(1,get_var(1) - 1);
			printf(1,"In child %d\n",get_var(1));
			release_lock(1);
			acquire_lock(0);
			set_var(0,get_var(0) - 1);
			release_lock(0);
			i++;
		}	
	}
	else
	{
		while(j < 30)
		{
			if(get_var(0) == 1) continue;
			acquire_lock(1);
			set_var(1,get_var(1) + 1);
			printf(1,"In parent %d\n",get_var(1));
			release_lock(1);
			acquire_lock(0);
			set_var(0,get_var(0) + 1);
			release_lock(0);
			j++;
		}	
	}


	if(ret == 0)
	{
		exit();
	}
	else
	{
		wait();
		exit();
	}
}