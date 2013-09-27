#include <vmalloc.h>
main()
{
	Vmalloc_t	*region;

	region = vmmopen("/tmp/north123",0,1024*1024);
	printf("%x\n",malloc(100));
}
