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

static Agraph_t *agfindsubg_by_id(Agraph_t * g, unsigned long id)
{
    Agraph_t template;

    agdtdisc(g, g->g_dict, &Ag_subgraph_id_disc);
    AGID(&template) = id;
    return (Agraph_t *) dtsearch(g->g_dict, &template);
}

static Agraph_t *localsubg(Agraph_t * g, unsigned long id)
{
    Agraph_t *subg;

    subg = agfindsubg_by_id(g, id);
    if (subg)
	return subg;

    subg = agalloc(g, sizeof(Agraph_t));
    subg->clos = g->clos;
    subg->desc = g->desc;
    subg->desc.maingraph = FALSE;
    subg->parent = g;
    subg->root = g->root;
    AGID(subg) = id;
    return agopen1(subg);
}

Agraph_t *agidsubg(Agraph_t * g, unsigned long id, int cflag)
{
    Agraph_t *subg;
    subg = agfindsubg_by_id(g, id);
    if ((subg == NILgraph) && cflag && agallocid(g, AGRAPH, id))
	subg = localsubg(g, id);
    return subg;
}

Agraph_t *agsubg(Agraph_t * g, char *name, int cflag)
{
    unsigned long id;
    Agraph_t *subg;

    if (name && agmapnametoid(g, AGRAPH, name, &id, FALSE)) {
	/* might already exist */
	if ((subg = agfindsubg_by_id(g, id)))
	    return subg;
    }

    if (cflag && agmapnametoid(g, AGRAPH, name, &id, TRUE))	/* reserve id */
	return localsubg(g, id);

    return NILgraph;
}

Agraph_t *agfstsubg(Agraph_t * g)
{
    return (Agraph_t *) dtfirst(g->g_dict);
}

Agraph_t *agnxtsubg(Agraph_t * subg)
{
    Agraph_t *g;

    g = agparent(subg);
    return (Agraph_t *) dtnext(g->g_dict, subg);
}

Agraph_t *agparent(Agraph_t * g)
{
    return g->parent;
}

Agraph_t *agroot(Agraph_t * g)
{
    return g->root;
}

/* this function is only responsible for deleting the entry
 * in the parent's subg dict.  the rest is done in agclose().
 */
long agdelsubg(Agraph_t * g, Agraph_t * subg)
{
    return (long) dtdelete(g->g_dict, subg);
}
