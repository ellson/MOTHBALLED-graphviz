/* vim:set shiftwidth=4 ts=4: */

#include <spinehdr.h>
#include <stdlib.h>
#include <subset.h>

static Void_t *mkPtrItem(Dt_t * d, ptritem * obj, Dtdisc_t * disc)
{
	NOTUSED(d);
	NOTUSED(disc);
	ptritem *np = NEW(ptritem);
	np->v = obj->v;
	return (Void_t *) np;
}

static void freePtrItem(Dt_t * d, ptritem * obj, Dtdisc_t * disc)
{
	NOTUSED(d);
	NOTUSED(disc);
	free(obj);
}

static int cmpptr(Dt_t * d, void **key1, void **key2, Dtdisc_t * disc)
{
	NOTUSED(d);
	NOTUSED(disc);
	if (*key1 > *key2)
		return 1;
	else if (*key1 < *key2)
		return -1;
	else
		return 0;
}

static Dtdisc_t ptrdisc = {
	offsetof(ptritem, v),
	sizeof(void *),
	offsetof(ptritem, link),
	(Dtmake_f) mkPtrItem,
	(Dtfree_f) freePtrItem,
	(Dtcompar_f) cmpptr,
	0,
	0,
	0
};

Dt_t *mkSubset()
{
	Dt_t *s = dtopen(&ptrdisc, Dtoset);
	return s;
}

void addSubset(Dt_t * s, void *n)
{
	ptritem dummy;

	dummy.v = n;
	dtinsert(s, &dummy);
}

int sizeSubset(Dt_t * s)
{
	return dtsize(s);
}

void clearSubset(Dt_t * s)
{
	dtclear(s);
}

void closeSubset(Dt_t * s)
{
	dtclose(s);
}

typedef struct {
	walkfn wf;
	void *state;
} auxstate;

static int auxfn(Dt_t * s, void *data, void *state)
{
	NOTUSED(s);
	return ((auxstate *) state)->wf(((ptritem *) data)->v,
									((auxstate *) state)->state);
}

void walkSubset(Dt_t * s, walkfn wf, void *state)
{
	auxstate xstate;

	xstate.wf = wf;
	xstate.state = state;
	dtwalk(s, auxfn, &xstate);
}
