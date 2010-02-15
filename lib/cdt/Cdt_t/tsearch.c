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

	/* testing Dtoset */
	dt = dtopen(&Disc,Dtoset);
	if((long)dtinsert(dt,7L) != 7)
		terror("Insert 7");
	if((long)dtinsert(dt,1L) != 1)
		terror("Insert 1");
	if((long)dtinsert(dt,3L) != 3)
		terror("Insert 3");
	if((long)dtinsert(dt,4L) != 4)
		terror("Insert 4");
	if((long)dtinsert(dt,2L) != 2)
		terror("Insert 2");
	if((long)dtinsert(dt,6L) != 6)
		terror("Insert 6");
	if((long)dtinsert(dt,7L) != 7)
		terror("Insert 7,2");

	if((long)dtmost(dt, 5L) != 4)
		terror("Should have found 4");
	if(dtfound(dt) )
		terror("Should not have found 5");
	if((long)dtleast(dt, 5L) != 6)
		terror("Should have found 6");
	if(dtfound(dt) )
		terror("Should not have found 5");

	if((long)dtinsert(dt,5L) != 5)
		terror("Insert 5");

	if((long)dtmost(dt, 5L) != 5)
		terror("Should have found 5");
	if(!dtfound(dt) )
		terror("Should have found 5");
	if((long)dtleast(dt, 3L) != 3)
		terror("Should have found 3");
	if(!dtfound(dt) )
		terror("Should have found 3");

	for(i = 1; i <= 7; ++i)
		if((long)dtsearch(dt,i) != i)
			terror("Dtoset search");
	for(link = dtflatten(dt), i = 1; link; link = dtlink(dt,link), i += 1)
		if((long)dtobj(dt,link) != i)
			terror("Dtoset flatten");
	for(i = (long)dtlast(dt), k = 7; k >= 1; i = (long)dtprev(dt,i), k -= 1)
		if(i != k)
			terror("Dtoset backwalk");

	/* test macro search function */
	if(dttreeset(dt,6,1) >= 0)
	{	for(i = 1; i <= 7; ++i)
		{	DTTREEMATCH(dt, i, k = (long) );
			if(i != k)
				terror("DTTREEMATCH() failed");
		}

		for(i = 1; i <= 7; ++i)
		{	DTTREESEARCH(dt, i, k = (long) );
			if(i != k)
				terror("DTTREESEARCH() failed");
		}
	}

	/* reverse ordering */
	dtdisc(dt,&Rdisc,0);
	for(i = 7; i >= 1; --i)
		if((long)dtsearch(dt,i) != i)
			terror("Dtoset search 2");
	for(i = (long)dtlast(dt), k = 1; k <= 7; i = (long)dtprev(dt,i), k += 1)
		if(i != k)
			terror("Dtoset backwalk 2");
	for(link = dtflatten(dt), i = 7; link; link = dtlink(dt,link), i -= 1)
		if((long)dtobj(dt,link) != i)
			terror("Dtoset flatten 2");

	if(!(link = dtextract(dt)) )
		terror("Fail extracting Dtoset");
	if(dtrestore(dt,link) < 0)
		terror("Fail restoring Dtoset");
	if(dtsize(dt) != 7)
		terror("Dtoset size after extract");
	for(link = dtflatten(dt), i = 7; link; link = dtlink(dt,link), i -= 1)
		if((long)dtobj(dt,link) != i)
			terror("Dtoset flatten after extract");

	/* change to hashing */
	dtmethod(dt,Dtset);
	for(i = 1; i <= 7; ++i)
		if((long)dtsearch(dt,i) != i)
			terror("Dtset search");
	for(link = dtflatten(dt), i = 0; link; link = dtlink(dt,link))
		i += 1;
	if(i != 7)
		terror("Dtset flatten");
	for(i = (long)dtlast(dt), k = 0; i != 0; i = (long)dtprev(dt,i))
		k += 1;
	if(k != 7)
		terror("Dtset flatten 2");

	if(!(link = dtextract(dt)) )
		terror("Fail extracting Dtset");
	if(dtrestore(dt,link) < 0)
		terror("Fail restoring Dtset");
	if(dtsize(dt) != 7)
		terror("Dtset size after extract");
	for(i = (long)dtlast(dt), k = 0; i != 0; i = (long)dtprev(dt,i))
		k += 1;
	if(k != 7)
		terror("Dtset flatten after extract");

	dtdisc(dt,&Disc,0);
	for(i = 1; i <= 7; ++i)
		if((long)dtsearch(dt,i) != i)
			terror("Dtset search 2");
	for(link = dtflatten(dt), i = 0; link; link = dtlink(dt,link))
		i += 1;
	if(i != 7)
		terror("Dtset flatten 2");

	dtclear(dt);
	if(dtsize(dt) != 0)
		terror("Dtsize");

	/* testing Dtlist */
	dtmethod(dt,Dtlist);
	if((long)dtinsert(dt,1) != 1)
		terror("Dtlist insert 1.1");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.1");
	if((long)dtinsert(dt,2) != 2)
		terror("Dtlist insert 2.1");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.2");
	if((long)dtinsert(dt,2) != 2)
		terror("Dtlist insert 2.2");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.3");
	if((long)dtinsert(dt,1) != 1)
		terror("Dtlist insert 1.2");

	/* check multiplicities */
	for(i = 1; i <= 3; ++i)
		count[i] = 0;
	for(i = (long)dtlast(dt); i != 0; i = (long)dtprev(dt,i))
		count[i] += 1;
	if(count[1] != 2)
		terror("Dtlist count 1");
	if(count[2] != 2)
		terror("Dtlist count 2");
	if(count[3] != 3)
		terror("Dtlist count 3");

	dtclear(dt);
	if(dtsize(dt) != 0)
		terror("Dtsize");

	/* testing Dtbag */
	dtmethod(dt,Dtbag);
	if((long)dtinsert(dt,1) != 1)
		terror("Dtlist insert 1.1");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.1");
	if((long)dtinsert(dt,2) != 2)
		terror("Dtlist insert 2.1");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.2");
	if((long)dtinsert(dt,2) != 2)
		terror("Dtlist insert 2.2");
	if((long)dtinsert(dt,3) != 3)
		terror("Dtlist insert 3.3");
	if((long)dtinsert(dt,1) != 1)
		terror("Dtlist insert 1.2");

	/* check multiplicities */
	for(i = 1; i <= 3; ++i)
		count[i] = 0;
	for(i = (long)dtlast(dt); i != 0; i = (long)dtprev(dt,i))
		count[i] += 1;
	if(count[1] != 2)
		terror("Dtbag count 1");
	if(count[2] != 2)
		terror("Dtbag count 2");
	if(count[3] != 3)
		terror("Dtbag count 3");

	/* test consecutive  1's */
	if((long)dtsearch(dt,1) != 1)
		terror("Dtbag search 1");
	if((long)dtnext(dt,1) != 1)
		terror("Dtbag next 1");
	if((long)dtnext(dt,1) == 1)
		terror("Dtbag next not expecting 1");

	/* test consecutive  2's */
	if((long)dtsearch(dt,2) != 2)
		terror("Dtbag search 2");
	if((long)dtnext(dt,2) != 2)
		terror("Dtbag next 2");
	if((long)dtnext(dt,2) == 2)
		terror("Dtbag next not expecting 2");

	/* test consecutive 3's */
	if((long)dtsearch(dt,3) != 3)
		terror("Dtbag search 3");
	if((long)dtnext(dt,3) != 3)
		terror("Dtbag next 3");
	if((long)dtnext(dt,3) != 3)
		terror("Dtbag next 3.2");
	if((long)dtnext(dt,3) == 3)
		terror("Dtbag next not expecting 3");

	/* change method to Dtobag */
	dtmethod(dt,Dtobag);

	/* check multiplicities */
	for(i = 1; i <= 3; ++i)
		count[i] = 0;
	for(i = (long)dtfirst(dt); i != 0; i = (long)dtnext(dt,i))
		count[i] += 1;
	if(count[1] != 2)
		terror("Dtobag count 1");
	if(count[2] != 2)
		terror("Dtobag count 2");
	if(count[3] != 3)
		terror("Dtobag count 3");

	/* test consecutive  1's */
	if((long)dtsearch(dt,1) != 1)
		terror("Dtobag search 1");
	if((long)dtnext(dt,1) != 1)
		terror("Dtobag next 1");
	if((long)dtnext(dt,1) != 2)
		terror("Dtobag next should be 2");

	/* test consecutive  2's */
	if((long)dtsearch(dt,2) != 2)
		terror("Dtobag search 2");
	if((long)dtnext(dt,2) != 2)
		terror("Dtobag next 2");
	if((long)dtnext(dt,2) != 3)
		terror("Dtobag next should be 3");

	/* test consecutive 3's */
	if((long)dtsearch(dt,3) != 3)
		terror("Dtobag search 3");
	if((long)dtnext(dt,3) != 3)
		terror("Dtobag next 3");
	if((long)dtnext(dt,3) != 3)
		terror("Dtobag next 3.2");
	if((long)dtnext(dt,3) == 3)
		terror("Dtobag next not expecting 3");

	/* test large set of values */
	dtclear(dt);
	dtmethod(dt,Dtset);
	for(i = 1; i < 20000; ++i)
		if((long)dtinsert(dt,i) != i)
			terror("Can't insert");
	dtmethod(dt,Dtoset);
	for(i = 1, k = (long)dtfirst(dt); i < 20000; ++i, k = (long)dtnext(dt,k))
		if(i != k)
			terror("Bad value");

	return 0;
}
