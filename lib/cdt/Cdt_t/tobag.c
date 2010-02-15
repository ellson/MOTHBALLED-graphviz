#include	"dttest.h"

Dtdisc_t Disc =
	{ 0, sizeof(long), -1,
	  newint, NIL(Dtfree_f), compare, hashint,
	  NIL(Dtmemory_f), NIL(Dtevent_f)
	};

Dtdisc_t Rdisc =
	{ 0, sizeof(long), -1,
	  newint, NIL(Dtfree_f), rcompare, hashint,
	  NIL(Dtmemory_f), NIL(Dtevent_f)
	};

main()
{
	Dt_t*		dt;
	Dtlink_t*	link;
	long		i, k, count[10];

	/* testing Dtobag */
	dt = dtopen(&Disc,Dtobag);
	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5.1");
	if((long)dtinsert(dt,2L) != 2)
		terror("Insert 2.1");
	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5.2");
	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order1\n");
	if((long)dtinsert(dt,3L) != 3)
		terror("Insert 3.1");
	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5.3");
	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order2\n");
	if((long)dtinsert(dt,4L) != 4)
		terror("Insert 4.1");
	if((long)dtinsert(dt,1L) != 1)
		terror("Insert 1");
	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order3\n");
	if((long)dtinsert(dt,2L) != 2)
		terror("Insert 2.2");
	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5.4");
	if((long)dtinsert(dt,4L) != 4)
		terror("Insert 4.2");
	if((long)dtinsert(dt,3L) != 3)
		terror("Insert 3.2");
	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order4\n");
	if((long)dtinsert(dt,4L) != 4)
		terror("Insert 4.3");
	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5.5");
	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order5\n");
	if((long)dtinsert(dt,3L) != 3)
		terror("Insert 3.3");
	if((long)dtinsert(dt,4L) != 4)
		terror("Insert 4.4");

	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order5\n");

	for(i = 0; i <= 5; ++i)
		count[i] = 0;
	for(i = (long)dtfirst(dt); i; i = (long)dtnext(dt,i))
		count[i] += 1;
	for(i = 0; i <= 5; ++i)
		if(count[i] != i)
			terror("Wrong count\n");

	for(i = 0; i <= 5; ++i)
		count[i] = 0;
	for(i = (long)dtlast(dt); i; i = (long)dtprev(dt,i))
		count[i] += 1;
	for(i = 0; i <= 5; ++i)
		if(count[i] != i)
			terror("Wrong count2\n");

	for(k = 0, i = (long)dtfirst(dt); i; k = i, i = (long)dtnext(dt,i))
		if(i < k)
			terror("Wrong order6\n");

	for(link = dtflatten(dt), i = 1; link; ++i)
	{	for(k = 1; k <= i; ++k, link = dtlink(dt,link))
			if(i != (long)dtobj(dt,link))
				terror("Bad element\n");
	}

	return 0;
}
