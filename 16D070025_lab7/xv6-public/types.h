typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

struct myspinlock
{
	uint locked;
};

int int_array[10];
struct myspinlock lock_array[10];