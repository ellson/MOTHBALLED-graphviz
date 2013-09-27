#include	<cdt.h>

/*	This program reads a list of words, 1 per line, from stdin,
**	removes any redundancies, then outputs the remainders in
**	alphabetic order.
*/

#define ulong	unsigned long
#define reg	register
#define NIL(t)	((t)0)

_BEGIN_EXTERNS_
extern Void_t*	malloc _ARG_((int));
extern Void_t*	memcpy _ARG_((Void_t*, const Void_t*, size_t));
extern void	free _ARG_((Void_t*));
_END_EXTERNS_

/* compare two strings by their alphabetic order */
#if __STD_C
static alphacmp(Dt_t* dt, reg Void_t* arg_s1, reg Void_t* arg_s2, Dtdisc_t* disc)
#else
static alphacmp(dt,arg_s1,arg_s2,disc)
Dt_t*		dt;
reg Void_t*	arg_s1;
reg Void_t*	arg_s2;
Dtdisc_t*	disc;
#endif
{
	reg int		c1, c2;
	reg char	*s1 = (char*)arg_s1, *s2 = (char*)arg_s2;

	while((c1 = *s1++) != 0)
	{	if((c2 = *s2++) == 0)
			return 1;

		if(c1 >= 'A' && c1 <= 'Z')
		{	if(c2 >= 'a' && c2 <= 'z')
			{	c2 = 'A' + (c2 - 'a');
				return c1 <= c2 ? -1 : 1;
			}
		}
		else if(c1 >= 'a' && c1 <= 'z')
		{	if(c2 >= 'A' && c2 <= 'Z')
			{	c2 = 'a' + (c2 - 'A');
				return c1 >= c2 ? 1 : -1;
			}
		}

		if((c1 -= c2) != 0)
			return c1;
	}

	return *s2 ? -1 : 0;
}

/* make a copy of a string */
#if __STD_C
static Void_t* newstring(Dt_t* dt, reg Void_t* s, Dtdisc_t* disc)
#else
static Void_t* newstring(dt,s,disc)
Dt_t*		dt;
reg Void_t*	s;
Dtdisc_t*	disc;
#endif
{
	reg Void_t*	news;
	reg int		n = strlen((char*)s)+1;

	if(!(news = malloc(n)) )
		return NIL(Void_t*);
	memcpy(news, s, n);

	return (Void_t*)news;
}

static Dtdisc_t	Disc =
	{ 0, 0,			/* Strings themselves are indexing keys
				   so "key" and "size" are set to zero.	*/
	  -1,			/* negative "link" means that strings
				   do not contain Dtlink_t and internal
				   dictionary holders will be allocated	*/
	  newstring,		/* duplicate strings so that they won't
				   be clobbered by further I/O.		*/
	  NIL(Dtfree_f),	/* does not free anything		*/
	  NIL(Dtcompar_f),	/* uses built-in string comparison 	*/
	  NIL(Dthash_f),	/* uses built-in string hashing		*/
	  NIL(Dtmemory_f),	/* uses default memory allocation	*/
	  NIL(Dtevent_f)	/* no event handling needed		*/
	};

main()
{
	char		s[1024];
	reg Dt_t*	dt;
	reg Dtlink_t*	link;

	/* create a dictionary, use a hash table for speed */
	if(!(dt = dtopen(&Disc,Dtset)) )
		return -1;

	/* read&insert strings into dt.
	   Yes, gets() should be avoided and something like Sfio's sfgetr()
	   should be used for robustness. But I am lazy...
	*/
	while(gets(s) )
		dtinsert(dt,s);

	/* at this point, strings in dt are unique.
	   Now change the comparison function to use alphabetic order.
	   At a discipline change, dtdisc() normally checks for new
	   duplicates and rehashes elements. Since alphacmp() and strcmp()
	   are equivalent with respect to distinguishing strings, DT_SAMECMP
	   is used to tell dtdisc() to skip reordering and checking for new
	   duplicates. This is ok because we are still in Dthash which does
	   not require ordering. Similarly, DT_SAMEHASH is used to assert that
	   there is no need to rehash elements.
	*/
	Disc.comparf = alphacmp;
	if(!dtdisc(dt,&Disc,DT_SAMECMP|DT_SAMEHASH))
		return -1;

	/* now order strings by switching to Dttree */
	if(!dtmethod(dt,Dtorder) )
		return -1;

	/* output the words in alphabetic order */
	for(link = dtflatten(dt); link; link = dtlink(dt,link))
		printf("%s\n",(char*)dtobj(dt,link));

	return 0;
}
