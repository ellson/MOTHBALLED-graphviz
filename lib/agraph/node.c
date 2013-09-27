/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#include "aghdr.h"

Agnode_t *agfindnode_by_id(Agraph_t * g, unsigned long id)
{
    Agnode_t *n;
    static Agnode_t template;

    template.base.tag.id = id;
    n = (Agnode_t *) dtsearch(g->n_id, &template);
    return n;
}

Agnode_t *agfindnode_by_name(Agraph_t * g, char *name)
{
    unsigned long id;

    if (agmapnametoid(g, AGNODE, name, &id, FALSE))
	return agfindnode_by_id(g, id);
    else
	return NILnode;
}

Agnode_t *agfstnode(Agraph_t * g)
{
    return (Agnode_t *) dtfirst(g->n_seq);
}

Agnode_t *agnxtnode(Agnode_t * n)
{
    Agraph_t *g;
    g = agraphof(n);
    if (agisflattened(g))
	return AGNEXTNODE(n);
    else {
	return (Agnode_t *) dtnext(g->n_seq, n);
    }
}

static Agnode_t *newnode(Agraph_t * g, unsigned long id, unsigned long seq)
{
    Agnode_t *n;

    n = agalloc(g, sizeof(Agnode_t));
    AGTYPE(n) = AGNODE;
    AGID(n) = id;
    AGSEQ(n) = seq;
    n->g = g;
    dtinsert(g->n_seq, n);
    dtinsert(g->n_id, n);
    return n;
}

/* create or bind the given node.  it may already exist in the parent. */
static Agnode_t *mklocalnode(Agraph_t * g, unsigned long id,
			     Agnode_t * rootnode, int *isnew)
{
    Agnode_t *n, *npar;
    Agraph_t *par;

    agnotflat(g);
    if (rootnode) {
	if (g->desc.maingraph)
	    return rootnode;
	if ((n = agfindnode_by_id(g, id)))
	    return n;
    }
    if ((par = agparent(g)))
	npar = mklocalnode(par, id, rootnode, isnew);
    else {
	npar = NILnode;
	*isnew = TRUE;
    }
    n = newnode(g, id, npar ? AGSEQ(npar) : agnextseq(g, AGNODE));
    if (npar)
	AGDATA(n) = AGDATA(npar);
    else {
	if (g->desc.has_attrs)
	    (void) agrealbindrec(n, AgDataRecName, sizeof(Agattr_t), FALSE,
				 TRUE);
	/* nodeattr_init and method_init will be called later */
    }
    return n;
}

static Agnode_t *localnode(Agraph_t * g, unsigned long id,
			   Agnode_t * rootnode)
{
    int isnew = FALSE;
    Agnode_t *n;
    n = mklocalnode(g, id, rootnode, &isnew);
    if (isnew) {
	if (g->desc.has_attrs)
	    agnodeattr_init(n, TRUE);
	agmethod_init(g, n);
    }
    return n;
}

Agnode_t *agidnode(Agraph_t * g, unsigned long id, int cflag)
{
    Agraph_t *root;
    Agnode_t *n, *rootnode;

    rootnode = NILnode;
    n = agfindnode_by_id(g, id);
    if ((n == NILnode) && cflag) {
	root = agroot(g);
	if (((g != root) && ((rootnode = agfindnode_by_id(agroot(g), id))))	/*old */
	    ||agallocid(g, AGNODE, id))	/* new */
	    n = localnode(g, id, rootnode);
    }
    return n;
}

Agnode_t *agnode(Agraph_t * g, char *name, int cflag)
{
    Agraph_t *root;
    Agnode_t *n, *rootnode;
    unsigned long id;

    if (agmapnametoid(g, AGNODE, name, &id, FALSE)) {
	/* might already exist locally */
	if ((n = agfindnode_by_id(g, id)))
	    return n;

	/* might already exist globally */
	root = agroot(g);
	if (cflag && (g != root)
	    && ((rootnode = agfindnode_by_id(root, id))))
	    if ((n = localnode(g, id, rootnode)))
		return n;
    }

    if (cflag && agmapnametoid(g, AGNODE, name, &id, TRUE))	/* reserve id */
	return localnode(g, id, NILnode);

    return NILnode;
}

/* removes image of node and its edges from graph.
   caller must ensure arg_n belongs to g. */
void agdelnodeimage(Agnode_t * n, void *ignored)
{
    Agraph_t *g;
    Agedge_t *e, *f;

    NOTUSED(ignored);
    g = agraphof(n);
    agnotflat(g);
    agflatten_edges(g, n);
#ifndef NOFLATOPT
    if (n->out)
	assert(AGPREV(n->out) == NILedge);
    for (e = AGFSTOUT(n); e; e = f) {
	f = AGNXTE(e);
	if (f)
	    assert(AGPREV(f) == e);
	if (e->node != n)
	    agedgesetop(g, AGOPP(e), FALSE);
	agfree(g, e);		/* Note, assumes edgepair layout in edge.c */
    }
    if (n->in)
	assert(AGPREV(n->in) == NILedge);
    for (e = AGFSTIN(n); e; e = f) {
	f = AGNXTE(e);
	if (f)
	    assert(AGPREV(f) == e);
	if (e->node != n)
	    agedgesetop(g, AGOPP(e), FALSE);
	agfree(g, AGIN2OUT(e));
    }
#else
    for (e = agfstedge(n); e; e = f) {
	f = agnxtedge(e, n);
	agdeledgepair(e);
    }
#endif
    dtdelete(g->n_seq, n);
    dtdelete(g->n_id, n);
    agfree(g, n);
}

int agdelnode(Agnode_t * n)
{
    Agraph_t *g;
    Agedge_t *e, *f;

    g = agraphof(n);
    if ((Agnode_t *) dtsearch(g->n_id, n) == NILnode)
	return FAILURE;		/* bad arg */
    if (agisarootobj(n)) {
	for (e = agfstedge(n); e; e = f) {
	    f = agnxtedge(e, n);
	    agdeledge(e);
	}
	if (g->desc.has_attrs)
	    agnodeattr_delete(n);
	agmethod_delete(g, n);
	agrecclose((Agobj_t *) n);
	agfreeid(g, AGNODE, AGID(n));
    }
    return agapply(g, (Agobj_t *) n, (agobjfn_t) agdelnodeimage, NILnode,
		   FALSE);
}

static void dict_relabel(Agnode_t * n, void *arg)
{
    Agraph_t *g;
    unsigned long new_id;

    g = agraphof(n);
    new_id = *(unsigned long *) arg;
    agnotflat(g);
    dtdelete(g->n_id, n);
    AGID(n) = new_id;
    dtinsert(g->n_id, n);
}

int agrelabel_node(Agnode_t * n, char *newname)
{
    Agraph_t *g;
    unsigned long new_id;

    g = agroot(agraphof(n));
    if (agfindnode_by_name(g, newname))
	return FAILURE;
    if (agmapnametoid(g, AGNODE, newname, &new_id, TRUE)) {
	if (agfindnode_by_id(agroot(g), new_id) == NILnode) {
	    agfreeid(g, AGNODE, AGID(n));
	    agapply(g, (Agobj_t *) n, (agobjfn_t) dict_relabel,
		    (void *) &new_id, FALSE);
	    return SUCCESS;
	} else {
	    agfreeid(g, AGNODE, new_id);	/* couldn't use it after all */
	}
    }
    return FAILURE;
}

/* return a node in <g> that is the image of <arg_n>. */
Agnode_t *agsubnode(Agraph_t * g, Agnode_t * arg_n, int cflag)
{
    Agnode_t *n, *npar;
    Agraph_t *par;

    n = agfindnode_by_id(g, AGID(arg_n));

    if ((n == NILnode) && cflag) {
	if ((par = agparent(g))) {
	    if ((npar = agsubnode(par, arg_n, cflag))) {
		n = newnode(g, AGID(npar), AGSEQ(npar));
		AGDATA(n) = AGDATA(npar);
/* #ifdef MTFHACK */
		n->base.tag.mtflock = npar->base.tag.mtflock;
/* #endif */
	    }			/* else someone handed us a flaky ptr in <n> */
	}
    }
    /* else probe failed */
    return n;
}
