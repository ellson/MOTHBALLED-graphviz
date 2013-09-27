#include	"dttest.h"

Dtdisc_t Disc =
	{ 0, sizeof(long), -1,
	  newint, NIL(Dtfree_f), compare, hashint,
	  NIL(Dtmemory_f), NIL(Dtevent_f)
	};

static int Count, See[10];

#if __STD_C
static visit(Dt_t* dt, Void_t* obj, Void_t* data)
#else
static visit(dt, obj, data)
Dt_t*	dt;
Void_t* obj;
Void_t*	data;
#endif
{
	See[(long)obj] = 1;
	Count += 1;
	return 0;
}

main()
{
	Dt_t		*dt1, *dt2;
	long		i;

	if(!(dt1 = dtopen(&Disc,Dtoset)) )
		terror("Opening Dtoset");
	if(!(dt2 = dtopen(&Disc,Dtoset)) )
		terror("Opening Dtoset");

	dtinsert(dt1,1L);
	dtinsert(dt1,3L);
	dtinsert(dt1,5L);
	dtinsert(dt1,2L);

	dtinsert(dt2,2L);
	dtinsert(dt2,4L);
	dtinsert(dt2,6L);
	dtinsert(dt2,3L);

	if((long)dtsearch(dt1,4L) != 0)
		terror("Finding 4 here?");

	dtview(dt1,dt2);
	if((long)dtsearch(dt1,4L) != 4)
		terror("Should find 4 here!");

	dtwalk(dt1,visit,NIL(Void_t*));
	if(Count != 6)
		terror("Walk wrong length");
	for(i = 1; i <= 6; ++i)
		if(!See[i] )
			terror("Bad walk");

	dtinsert(dt1,2L);

	Count = 0;
	for(i = (long)dtfirst(dt1); i; i = (long)dtnext(dt1,i))
		Count++;
	if(Count != 6)
		terror("Walk wrong length2");

	Count = 0;
	for(i = (long)dtlast(dt1); i; i = (long)dtprev(dt1,i))
		Count++;
	if(Count != 6)
		terror("Walk wrong length3");

	/* dt1: 1 3 5 2
	   dt2: 2 4 6 3
	*/
	Count = 0;
	dtmethod(dt1,Dtset);
	dtmethod(dt2,Dtset);
	for(i = (long)dtfirst(dt1); i; i = (long)dtnext(dt1,i))
		Count++;
	if(Count != 6)
		terror("Walk wrong length4");

	return 0;
}
