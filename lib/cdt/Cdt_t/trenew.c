#include	"dttest.h"

typedef struct _obj_s
{	Dtlink_t	link;
	long		key;
} Obj_t;

#if __STD_C
static int mycompare(Dt_t* dt, Void_t* o1, Void_t* o2, Dtdisc_t* disc)
#else
static int mycompare(dt, o1, o2, disc)
Dt_t*		dt;
Void_t* 	o1;
Void_t* 	o2;
Dtdisc_t*	disc;
#endif
{
	return (int)(((Obj_t*)o1)->key - ((Obj_t*)o2)->key);
}

#if __STD_C
static unsigned int myhash(Dt_t* dt, Void_t* o, Dtdisc_t* disc)
#else
static unsigned int myhash(dt, o, disc)
Dt_t*		dt;
Void_t*		o;
Dtdisc_t*	disc;
#endif
{
	return (unsigned int)((Obj_t*)o)->key;
}

Dtdisc_t Disc =
	{ 0, 0, 0,
	  NIL(Dtmake_f), NIL(Dtfree_f),
	  mycompare, myhash,
	  NIL(Dtmemory_f), NIL(Dtevent_f)
	};


main()
{
	Dt_t*	dt;
	Obj_t	*obj, o[6];
	long	i;

	o[0].key = 1;
	o[1].key = 3;
	o[2].key = 5;
	o[3].key = 7;
	o[4].key = 9;
	o[5].key = 11;

	dt = dtopen(&Disc,Dtorder);
	dtinsert(dt,&o[0]);
	dtinsert(dt,&o[2]);
	dtinsert(dt,&o[4]);
	dtinsert(dt,&o[3]);
	dtinsert(dt,&o[1]);
	dtinsert(dt,&o[5]);

	if(dtrenew(dt,&o[0]) )
		terror("Dtorder: can't renew yet");

	if((Obj_t*)dtsearch(dt,&o[5]) != &o[5])
		terror("Dtorder: search failed");
	o[5].key = 4;
	if((Obj_t*)dtrenew(dt,&o[5]) != &o[5] )
		terror("Dtorder: renew failed");
	if((Obj_t*)dtnext(dt,dtnext(dt,dtfirst(dt))) != &o[5])
		terror("Dtorder: wrong order after renew");

	dtmethod(dt,Dtset);
	if((Obj_t*)dtsearch(dt,&o[5]) != &o[5])
		terror("Dtset: search failed");
	o[5].key = 11;
	if((Obj_t*)dtrenew(dt,&o[5]) != &o[5] )
		terror("Dtset: renew failed");
	dtmethod(dt,Dtorder);
	if((Obj_t*)dtlast(dt) != &o[5])
		terror("Dtset: wrong order");
	dtclose(dt);

	o[0].key = 1;
	o[1].key = 2;
	o[2].key = 3;
	o[3].key = 4;
	o[4].key = 5;
	o[5].key = 6;
	dt = dtopen(&Disc, Dtlist);
	dtinsert(dt,&o[5]);
	dtinsert(dt,&o[3]);
	dtinsert(dt,&o[1]);

	dtsearch(dt,&o[5]);
	dtinsert(dt,&o[4]);
	dtsearch(dt,&o[3]);
	dtinsert(dt,&o[2]);
	dtsearch(dt,&o[1]);
	dtinsert(dt,&o[0]);
	obj = (Obj_t*)dtfirst(dt);
	for(i = 1; obj; obj = (Obj_t*)dtnext(dt,obj), i += 1)
		if(obj->key != i)
			terror("Dtlist: wrong order");
	return 0;
}
