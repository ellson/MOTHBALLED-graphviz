/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include <cghdr.h>

int agdelete(Agraph_t * g, void *obj)
{
    Agraph_t *h;

    h = agraphof(obj);
    if ((g != h)
	&& ((AGTYPE((Agobj_t *) obj) != AGRAPH) || (g != agparent(h))))
	agerr(AGERR, "agdelete on wrong graph");

    switch (AGTYPE((Agobj_t *) obj)) {
    case AGNODE:
	return agdelnode(g, obj);
    case AGINEDGE:
    case AGOUTEDGE:
	return agdeledge(g, obj);
    case AGRAPH:
	return agclose(obj);
    default:
	agerr(AGERR, "agdelete on bad object");
    }
    return SUCCESS;		/* not reached */
}

int agrename(Agobj_t * obj, char *newname)
{
    Agraph_t *g;
    unsigned long old_id, new_id;

    switch (AGTYPE(obj)) {
    case AGRAPH:
	old_id = AGID(obj);
	g = agraphof(obj);
	/* can we reserve the id corresponding to newname? */
	if (agmapnametoid(agroot(g), AGTYPE(obj), newname,
			  &new_id, FALSE) == 0)
	    return FAILURE;
	if (new_id == old_id)
	    return SUCCESS;
	if (agmapnametoid(agroot(g), AGTYPE(obj), newname,
			  &new_id, TRUE) == 0)
	    return FAILURE;
	if (agparent(g) && agidsubg(agparent(g), new_id, 0))
	    return FAILURE;
	agfreeid(g, AGRAPH, old_id);
	AGID(g) = new_id;
	break;
    case AGNODE:
	return agrelabel_node((Agnode_t *) obj, newname);
	agrename(obj, newname);
	break;
    case AGINEDGE:
    case AGOUTEDGE:
	return FAILURE;
    }
    return SUCCESS;
}

/* perform initialization/update/finalization method invocation.
 * skip over nil pointers to next method below.
 */

void agmethod_init(Agraph_t * g, void *obj)
{
    if (g->clos->callbacks_enabled)
	aginitcb(g, obj, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_INITIALIZE, NILsym);
}

void aginitcb(Agraph_t * g, void *obj, Agcbstack_t * cbstack)
{
    agobjfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    aginitcb(g, obj, cbstack->prev);
    fn = NIL(agobjfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.ins;
	break;
    case AGNODE:
	fn = cbstack->f->node.ins;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.ins;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state);
}

void agmethod_upd(Agraph_t * g, void *obj, Agsym_t * sym)
{
    if (g->clos->callbacks_enabled)
	agupdcb(g, obj, sym, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_UPDATE, sym);
}

void agupdcb(Agraph_t * g, void *obj, Agsym_t * sym, Agcbstack_t * cbstack)
{
    agobjupdfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    agupdcb(g, obj, sym, cbstack->prev);
    fn = NIL(agobjupdfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.mod;
	break;
    case AGNODE:
	fn = cbstack->f->node.mod;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.mod;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state, sym);
}

void agmethod_delete(Agraph_t * g, void *obj)
{
    if (g->clos->callbacks_enabled)
	agdelcb(g, obj, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_DELETION, NILsym);
}

void agdelcb(Agraph_t * g, void *obj, Agcbstack_t * cbstack)
{
    agobjfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    agdelcb(g, obj, cbstack->prev);
    fn = NIL(agobjfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.del;
	break;
    case AGNODE:
	fn = cbstack->f->node.del;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.del;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state);
}

Agraph_t *agroot(void* obj)
{
    switch (AGTYPE(obj)) {
    case AGINEDGE:
    case AGOUTEDGE:
	return ((Agedge_t *) obj)->node->root;
    case AGNODE:
	return ((Agnode_t *) obj)->root;
    case AGRAPH:
	return ((Agraph_t *) obj)->root;
    default:			/* actually can't occur if only 2 bit tags */
	agerr(AGERR, "agroot of a bad object");
	return NILgraph;
    }
}

Agraph_t *agraphof(void *obj)
{
    switch (AGTYPE(obj)) {
    case AGINEDGE:
    case AGOUTEDGE:
	return ((Agedge_t *) obj)->node->root;
    case AGNODE:
	return ((Agnode_t *) obj)->root;
    case AGRAPH:
	return (Agraph_t *) obj;
    default:			/* actually can't occur if only 2 bit tags */
	agerr(AGERR, "agraphof a bad object");
	return NILgraph;
    }
}

/* to manage disciplines */
void agpushdisc(Agraph_t * g, Agcbdisc_t * cbd, void *state)
{
    Agcbstack_t *stack_ent;

    stack_ent = AGNEW(g, Agcbstack_t);
    stack_ent->f = cbd;
    stack_ent->state = state;
    stack_ent->prev = g->clos->cb;
    g->clos->cb = stack_ent;
}

int agpopdisc(Agraph_t * g, Agcbdisc_t * cbd)
{
    Agcbstack_t *stack_ent;

    stack_ent = g->clos->cb;
    if (stack_ent) {
	if (stack_ent->f == cbd)
	    g->clos->cb = stack_ent->prev;
	else {
	    while (stack_ent && (stack_ent->prev->f != cbd))
		stack_ent = stack_ent->prev;
	    if (stack_ent && stack_ent->prev)
		stack_ent->prev = stack_ent->prev->prev;
	}
	if (stack_ent) {
	    agfree(g, stack_ent);
	    return SUCCESS;
	}
    }
    return FAILURE;
}

void *aggetuserptr(Agraph_t * g, Agcbdisc_t * cbd)
{
    Agcbstack_t *stack_ent;

    for (stack_ent = g->clos->cb; stack_ent; stack_ent = stack_ent->prev)
	if (stack_ent->f == cbd)
	    return stack_ent->state;
    return NIL(void *);
}

int agobjkind(void *arg)
{
	return AGTYPE(arg);
}
