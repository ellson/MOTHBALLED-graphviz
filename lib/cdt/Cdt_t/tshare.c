#include	"dttest.h"

static Void_t*	Space[1024];
static char*	Current = (char*)(&Space[0]);

#if __STD_C
static int event(Dt_t* dt, int type, Void_t* obj, Dtdisc_t* disc)
#else
static int event(dt, type, obj, disc)
Dt_t*	dt;
int	type;
Void_t* obj;
Dtdisc_t* disc;
#endif
{	if(type != DT_OPEN)
		return 0;

	/* opening first dictionary */
	if(Current == (char*)(&Space[0]))
		return 0;
	else /* opening a dictionary sharing with some previous one */
	{	*((Void_t**)obj) = (Void_t*)(&Space[0]);
		return 1;
	}
}

#if __STD_C
static Void_t* memory(Dt_t* dt, Void_t* buf, size_t size, Dtdisc_t* disc)
#else
static Void_t* memory(dt,buf,size,disc)
Dt_t*	dt;
Void_t* buf;
size_t	size;
Dtdisc_t* disc;
#endif
{
	if(!buf)
	{	size = ((size + sizeof(Void_t*)-1)/sizeof(Void_t*))*sizeof(Void_t*);
		buf = (Void_t*)Current;
		Current += size;
	}
	return buf;
}

Dtdisc_t Disc =
	{ 0, sizeof(long), -1,
	  newint, NIL(Dtfree_f), compare, hashint,
	  memory, event
	};

main()
{
	Dt_t		*dt1, *dt2;
	long		i, k;

	if(!(dt1 = dtopen(&Disc,Dtorder)) )
		terror("Opening Dtorder1");
	if((long)dtinsert(dt1,1L) != 1)
		terror("Inserting 1");
	if((long)dtinsert(dt1,3L) != 3)
		terror("Inserting 3");
	if((long)dtinsert(dt1,5L) != 5)
		terror("Inserting 5");

	if(!(dt2 = dtopen(&Disc,Dtorder)) )
		terror("Opening Dtorder2");
	if((long)dtinsert(dt2,2L) != 2)
		terror("Inserting 2");
	if((long)dtinsert(dt2,4L) != 4)
		terror("Inserting 4");
	if((long)dtinsert(dt2,6L) != 6)
		terror("Inserting 6");

	for(i = 1; i <= 6; ++i)
		if((long)dtsearch(dt1,i) != i)
			terror("Didn't find a long");

	for(i = (long)dtlast(dt2), k = 6; i != 0; i = (long)dtprev(dt2,i), k -= 1)
		if(i != k)
			terror("Didn't walk a long");

	return 0;
}
