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


#define EXTERN
#include "aghdr.h"

const char AgraphVersion[] = VERSION;

/*
 * this code sets up the resource management discipline
 * and returns a new main graph struct.
 */
static Agclos_t *agclos(Agdisc_t * proto)
{
    Agmemdisc_t *memdisc;
    void *memclosure;
    Agclos_t *rv;

    /* establish an allocation arena */
    memdisc = ((proto && proto->mem) ? proto->mem : &AgMemDisc);
    memclosure = memdisc->open();
    rv = memdisc->alloc(memclosure, sizeof(Agclos_t));
    rv->disc.mem = memdisc;
    rv->state.mem = memclosure;
    rv->disc.id = ((proto && proto->id) ? proto->id : &AgIdDisc);
    rv->disc.io = ((proto && proto->io) ? proto->io : &AgIoDisc);
    rv->callbacks_enabled = TRUE;
    return rv;
}

/*
 * Open a new main graph with the given descriptor (directed, strict, etc.)
 */
Agraph_t *agopen(char *name, Agdesc_t desc, Agdisc_t * arg_disc)
{
    Agraph_t *g;
    Agclos_t *clos;
    unsigned long gid;

    clos = agclos(arg_disc);
    g = clos->disc.mem->alloc(clos->state.mem, sizeof(Agraph_t));
    AGTYPE(g) = AGRAPH;
    g->clos = clos;
    g->desc = desc;
    g->desc.maingraph = TRUE;
    g->root = g;
    g->clos->state.id = g->clos->disc.id->open(g);
    if (agmapnametoid(g, AGRAPH, name, &gid, TRUE))
	AGID(g) = gid;
    /* else AGID(g) = 0 because we have no alternatives */
    return agopen1(g);
}

/* 
 * initialize dictionaries, set seq, invoke init method of new graph
 */
Agraph_t *agopen1(Agraph_t * g)
{
    Agraph_t *par;

    g->n_seq = agdtopen(g, &Ag_obj_seq_disc, Dttree);
    g->n_id = agdtopen(g, &Ag_obj_id_disc, Dttree);
    g->e_seq = agdtopen(g, &Ag_obj_seq_disc, Dttree);
    g->e_id = agdtopen(g, &Ag_edge_disc, Dttree);
    g->g_dict = agdtopen(g, &Ag_obj_id_disc, Dttree);

    par = agparent(g);
    if (par) {
	AGSEQ(g) = agnextseq(par, AGRAPH);
	dtinsert(par->g_dict, g);
    }				/* else AGSEQ=0 */
    if (g->desc.has_attrs)
	agraphattr_init(g, FALSE);
    agmethod_init(g, g);
    return g;
}

/*
 * Close a graph or subgraph, freeing its storage.
 */
int agclose(Agraph_t * g)
{
    Agraph_t *subg, *next_subg, *par;
    Agnode_t *n, *next_n;

    agflatten(g, FALSE);
    par = agparent(g);

    if ((par == NILgraph) && (AGDISC(g, mem)->close)) {
	/* free entire heap */
	agmethod_delete(g, g);	/* invoke user callbacks */
	agfreeid(g, AGRAPH, AGID(g));
	AGDISC(g, mem)->close(AGCLOS(g, mem));	/* whoosh */
	return SUCCESS;
    }

    for (subg = agfstsubg(g); subg; subg = next_subg) {
	next_subg = agnxtsubg(subg);
	agclose(subg);
    }

    for (n = agfstnode(g); n; n = next_n) {
	next_n = agnxtnode(n);
	agdelnode(n);
    }

    aginternalmapclose(g);
    agmethod_delete(g, g);

    assert(dtsize(g->n_id) == 0);
    agdtclose(g, g->n_id);
    assert(dtsize(g->n_seq) == 0);
    agdtclose(g, g->n_seq);

    assert(dtsize(g->e_id) == 0);
    agdtclose(g, g->e_id);
    assert(dtsize(g->e_seq) == 0);
    agdtclose(g, g->e_seq);

    assert(dtsize(g->g_dict) == 0);
    agdtclose(g, g->g_dict);

    if (g->desc.has_attrs)
	agraphattr_delete(g);
    agrecclose((Agobj_t *) g);
    agfreeid(g, AGRAPH, AGID(g));

    if (par) {
	agdelsubg(par, g);
	agfree(par, g);
    } else {
	Agmemdisc_t *memdisc;
	void *memclos, *clos;
	while (g->clos->cb)
	    agpopdisc(g, g->clos->cb->f);
	AGDISC(g, id)->close(AGCLOS(g, id));
	agstrclose(g);
	memdisc = AGDISC(g, mem);
	memclos = AGCLOS(g, mem);
	clos = g->clos;
	(memdisc->free) (memclos, g);
	(memdisc->free) (memclos, clos);
    }
    return SUCCESS;
}

unsigned long agnextseq(Agraph_t * g, int objtype)
{
    return ++(g->clos->seq[objtype]);
}

int agnnodes(Agraph_t * g)
{
    return dtsize(g->n_id);
}

int agnedges(Agraph_t * g)
{
    Agnode_t *n;
    int rv = 0;

    for (n = agfstnode(g); n; n = agnxtnode(n))
	rv += agdegree(n, FALSE, TRUE);	/* must use OUT to get self-arcs */
    return rv;
}


int agisflattened(Agraph_t * g)
{
    return g->desc.flatlock;
}

int agisdirected(Agraph_t * g)
{
    return g->desc.directed;
}

int agisundirected(Agraph_t * g)
{
    return NOT(agisdirected(g));
}

int agisstrict(Agraph_t * g)
{
    return g->desc.strict;
}

int agdegree(Agnode_t * n, int want_in, int want_out)
{
    Agedge_t *e;
    int rv = 0;

    if (want_in)
	for (e = agfstin(n); e; e = agnxtin(e))
	    rv++;
    if (want_out)
	for (e = agfstout(n); e; e = agnxtout(e))
	    rv++;
    return rv;
}


/* directed, strict, flatlock, maingraph */
Agdesc_t Agdirected = { 1, 0, 0, 1 };
Agdesc_t Agstrictdirected = { 1, 1, 0, 1 };
Agdesc_t Agundirected = { 0, 0, 0, 1 };
Agdesc_t Agstrictundirected = { 0, 1, 0, 1 };

Agdisc_t AgDefaultDisc = { &AgMemDisc, &AgIdDisc, &AgIoDisc };
