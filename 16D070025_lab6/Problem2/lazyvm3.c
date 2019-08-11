#include "types.h"
#include "stat.h"
#include "user.h"

#define ARRAY_SIZE 2048
int buf[ARRAY_SIZE];

#define EXTRA_SPACE 8182

int getpusz()
{
	/* todo */
	uint KERNBASE = 0x80000000;
	uint PGSIZE = 4096;

	uint initial = 0x00000000;

	uint mask1 = 0xFFC00000;
	// uint mask2 = 0xFFFFF000;
	uint inc1 = 0x00400000;
	uint inc2 = 0x00001000;
	int count = 0;

	while(initial < KERNBASE)
	{
		uint va1 = initial;

		uint pa; int flags;

		int ret1 = get_va_to_pa(va1,&pa,&flags);

		if(ret1 == 1)
		{
			uint va2 = va1;
			while(va2 <= ((va1 & mask1) | 0x003FF000))
			{
				int r = get_va_to_pa(va2,&pa,&flags);
				if(r == 1)
				{
					count ++;
				}
				va2 += inc2;
			}
		}

		initial += inc1;
	}

	return (count*PGSIZE);
}


void print_status()
{
	int vp;
	int pp;
	int sz;

	sz = getpsz();
	vp = sz / 4096;
	if((sz % 4096) != 0)
		vp += 1;
	pp = getpusz() / 4096;

	printf(1, "sz: %d, vp: %d, pp: %d\n", sz, vp, pp);
}


int
main(int argc, char *argv[])
{	
	buf[ARRAY_SIZE-1] = 1234;

	print_status();

	int * ptr = (int *)&buf[ARRAY_SIZE-1];
	printf(1, "addr: %d, value: %d\n", ((int)ptr), *ptr);

	sbrk(EXTRA_SPACE);

	print_status();

	ptr = (int *)20484;
	printf(1, "addr: %d, value: %d\n", ((int)ptr), *ptr);

	print_status();

	ptr = (int *)28658;
	printf(1, "addr: %d, value: %d\n", ((int)ptr), *ptr);

	print_status();

	ptr = (int *)28668;
	printf(1, "addr: %d, value: %d\n", ((int)ptr), *ptr);

	ptr = (int *)28670;
	printf(1, "addr: %d\n", ((int)ptr));
	printf(1, "value: %d\n", *ptr);

	print_status();
	
	exit();
}