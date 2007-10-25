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

#define EXTERN
#include <cghdr.h>

const char AgraphVersion[] = "VERSION";

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

    g->n_seq = agdtopen(g, &Ag_subnode_seq_disc, Dttree);
    g->n_id = agdtopen(g, &Ag_subnode_id_disc, Dttree);
    g->e_seq = agdtopen(g, g == agroot(g)? &Ag_mainedge_seq_disc : &Ag_subedge_seq_disc, Dttree);
    g->e_id = agdtopen(g, g == agroot(g)? &Ag_mainedge_id_disc : &Ag_subedge_id_disc, Dttree);
    g->g_dict = agdtopen(g, &Ag_subgraph_id_disc, Dttree);

    par = agparent(g);
    if (par) {
	AGSEQ(g) = agnextseq(par, AGRAPH);
	dtinsert(par->g_dict, g);
    }				/* else AGSEQ=0 */
    if (g->desc.has_attrs)
	agraphattr_init(g);
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
	next_n = agnxtnode(g, n);
	agdelnode(g, n);
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

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	rv += agdegree(g, n, FALSE, TRUE);	/* must use OUT to get self-arcs */
    return rv;
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

int agdegree(Agraph_t * g, Agnode_t * n, int want_in, int want_out)
{
    Agedge_t *e;
    int rv = 0;

    if (want_in)
	for (e = agfstin(g, n); e; e = agnxtin(g, e))
	    rv++;
    if (want_out)
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    rv++;
    return rv;
}

int agraphidcmpf(Dict_t * d, void *arg0, void *arg1, Dtdisc_t * disc)
{
    Agraph_t *sg0, *sg1;
    sg0 = (Agraph_t *) arg0;
    sg1 = (Agraph_t *) arg1;
    return (AGID(sg0) - AGID(sg1));
}

int agraphseqcmpf(Dict_t * d, void *arg0, void *arg1, Dtdisc_t * disc)
{
    Agraph_t *sg0, *sg1;
    sg0 = (Agraph_t *) arg0;
    sg1 = (Agraph_t *) arg1;
    return (AGSEQ(sg0) - AGSEQ(sg1));
}

Dtdisc_t Ag_subgraph_id_disc = {
    0,				/* pass object ptr  */
    0,				/* size (ignored)   */
    offsetof(Agraph_t, link),	/* link offset */
    NIL(Dtmake_f),
    NIL(Dtfree_f),
    agraphidcmpf,
    NIL(Dthash_f),
    agdictobjmem,
    NIL(Dtevent_f)
};


/* directed, strict, maingraph */
Agdesc_t Agdirected = { 1, 0, 1 };
Agdesc_t Agstrictdirected = { 1, 1, 1 };
Agdesc_t Agundirected = { 0, 0, 1 };
Agdesc_t Agstrictundirected = { 0, 1, 1 };

Agdisc_t AgDefaultDisc = { &AgMemDisc, &AgIdDisc, &AgIoDisc };
