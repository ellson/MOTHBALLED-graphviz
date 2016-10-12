/* vim:set shiftwidth=4 ts=4 */

#include <spinehdr.h>
#include <stdlib.h>
#include <subset.h>

static void *mkPtrItem(Dt_t * d, ptritem * obj, Dtdisc_t * disc)
{
	NOTUSED(d);
	NOTUSED(disc);
	ptritem *np = NEW(ptritem);
	np->v = obj->v;
	return (void *) np;
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

void* inSubset(Dt_t * s, void *n)
{
	return dtmatch(s, &n);
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
	Dt_t* s;
    int sz;
} setsize_t;

static int union_fn(Agnode_t * n, setsize_t *state)
{
	if (!inSubset(state->s, n))
		state->sz++;
	return 0;
}

int union_size(Dt_t* s0, Dt_t* s1)
{
	setsize_t state;

    state.s = s0;
	state.sz = sizeSubset(s0);
	walkSubset(s1, (walkfn)union_fn, &state);
	return state.sz;
}

static int intersect_fn(Agnode_t * n, setsize_t *state)
{
	if (inSubset(state->s, n))
		state->sz++;
	return 0;
}

int intersect_size(Dt_t* s0, Dt_t* s1)
{
	setsize_t state;

    state.s = s0;
	state.sz = 0;
	walkSubset(s1, (walkfn)intersect_fn, &state);
	return state.sz;
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
