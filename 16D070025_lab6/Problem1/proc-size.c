#include "types.h"
#include "stat.h"
#include "user.h"
char b[1024];

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

int getpksz()
{
	/* todo */
	uint KERNBASE = 0x80000000;
	uint PGSIZE = 4096;
    uint PHYSTOP = 0xFFC00000;

	uint initial = KERNBASE;

	uint mask1 = 0xFFC00000;
	// uint mask2 = 0xFFFFF000;
	uint inc1 = 0x00400000;
	uint inc2 = 0x00001000;
	int count = 0;

	while(initial <= PHYSTOP)
	{
		uint va1 = initial;

		uint pa; int flags;

		int ret1 = get_va_to_pa(va1,&pa,&flags);

		int b = 0;

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
				if(va2 == 0xFFFFF000) 
				{
					break; b = 1;	
				}
				va2 += inc2;
			}
		}

		if(b == 1) break;

		if(initial == PHYSTOP) break;

		initial += inc1;
	}

	return (count*PGSIZE);	
}

int
main(int argc, char *argv[])
{
	char *buf;

	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	buf=sbrk(4096);
	buf[0]='\0';
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	
	buf=sbrk(4096*7);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(1);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(2);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	exit();
}