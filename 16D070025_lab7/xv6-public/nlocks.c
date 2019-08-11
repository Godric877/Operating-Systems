#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int ret;

	init_counters();

	for(int i=0;i<10;i++)
	{
		ret = fork();
		if(ret == 0)
		{
			for(int j=0;j<1000;j++)
			{
				acquire_lock(i);
				set_var(i,get_var(i)+1);
				release_lock(i);
			}
			exit();
		}
	}

	for(int i=0;i<10;i++) wait();

	for(int i=0;i<1000;i++)
	{
		for(int j=0;j<10;j++)
		{
		acquire_lock(j);
		set_var(j,get_var(j)+1);
		release_lock(j);
		}
	}

	for(int i=0;i<10;i++)
	{
		printf(1,"data[%d] = %d\n",i,get_var(i));
	}

	exit();
}