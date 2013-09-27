#include	"dttest.h"

static int Pevent;
static int Event;
static int Hinit;

#if __STD_C
static int event(Dt_t* dt, int type, Void_t* obj, Dtdisc_t* disc)
#else
static int event(dt, type, obj, disc)
Dt_t*		dt;
int		type;
Void_t* 	obj;
Dtdisc_t* 	disc;
#endif
{
	Pevent = Event;
	Event = type;

	if(type == DT_HASHSIZE)
	{	Hinit += 1;
		*(ssize_t*)obj = 1024;
		return 1;
	}

	return 0;
}

Dtdisc_t Disc =
	{ 0, sizeof(long), -1,
	  newint, NIL(Dtfree_f), compare, hashint,
	  NIL(Dtmemory_f), event
	};

main()
{
	Dt_t		*dt;
	long		k;

	if(!(dt = dtopen(&Disc,Dtset)) )
		terror("Opening Dtset");
	if(Pevent != DT_OPEN && Event != DT_ENDOPEN)
		terror("No open event");

	dtmethod(dt,Dtoset);
	if(Event != DT_METH)
		terror("No meth event");

	dtdisc(dt,&Disc,0);
	if(Event != DT_DISC)
		terror("No disc event");

	dtclose(dt);
	if(Pevent != DT_CLOSE && Event != DT_ENDCLOSE)
		terror("No close event");

	if(!(dt = dtopen(&Disc,Dtset)) )
		terror("Opening Dtset");

	Pevent = Event = 0;
	for(k = 1; k <= 3000; ++k)
		dtinsert(dt, (Void_t*)k);
	if(Hinit != 1)
		terror("Wrong number of hash table events");

	return 0;
}
