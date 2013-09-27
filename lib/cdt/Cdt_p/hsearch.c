#include	<search.h>
#include	"../dthdr.h"

/*	POSIX hsearch library based on libdt
**	Written by Kiem-Phong Vo (AT&T Bell Labs, 07/19/95)
*/

/* type of objects in hash table */
typedef struct _hash_s
{	Dtlink_t	link;
	ENTRY		item;
} Hash_t;

/* object delete function */
#if __STD_C
static void hashfree(Dt_t* dt, Void_t* obj, Dtdisc_t* disc)
#else
static void hashfree(dt, obj, disc)
Dt_t*		dt;
Void_t*		obj;
Dtdisc_t*	disc;
#endif
{
	free(((Hash_t*)obj)->item.key);
	free(obj);
}

static Dt_t*	Hashtab;	/* object dictionary	*/
static Dtdisc_t	Hashdisc =	/* discipline		*/
{	sizeof(Dtlink_t), -1,
	0,
	NIL(Dtmake_f), hashfree,
	NIL(Dtcompar_f),	/* always use strcmp	*/
	NIL(Dthash_f),
	NIL(Dtmemory_f),
	NIL(Dtevent_f)
};

#if __STD_C
int hcreate(size_t nel)
#else
int hcreate(nel)
size_t	nel;
#endif
{
	if(Hashtab)	/* already opened */
		return 0;

	if(!(Hashtab = dtopen(&Hashdisc,Dtset)) )
		return 0;

	return 1;
}

void hdestroy()
{	if(Hashtab)
		dtclose(Hashtab);
	Hashtab = NIL(Dt_t*);
}

#if __STD_C
ENTRY* hsearch(ENTRY item, ACTION action)
#else
ENTRY* hsearch(item, action)
ENTRY	item;
ACTION	action;
#endif
{
	reg Hash_t*	o;

	if(!Hashtab)
		return NIL(ENTRY*);

	if(!(o = (Hash_t*)dtmatch(Hashtab,item.key)) && action == ENTER &&
	   (o = (Hash_t*)malloc(sizeof(Hash_t)) ) )
	{	o->item = item;
		o = (Hash_t*)dtinsert(Hashtab,o);
	}

	return o ? &(o->item) : NIL(ENTRY*);
}
