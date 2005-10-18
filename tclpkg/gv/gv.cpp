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

#define BUILTINS
#include "gvc.h"

GVC_t *gvc;

static void gvinit()
{
    gvc = gvContext();
}

Agraph_t *digraph(char *name)
{
    Agraph_t *g;

    if (!gvc)
	gvinit();
    g = agopen(name, AGDIGRAPH);
    GD_drawing(g) = NULL;
    return g;
}

Agraph_t *digraphstrict(char *name)
{
    Agraph_t *g;

    if (!gvc)
	gvinit();
    g = agopen(name, AGDIGRAPHSTRICT);
    GD_drawing(g) = NULL;
    return g;
}

Agraph_t *graph(char *name)
{
    Agraph_t *g;

    if (!gvc)
	gvinit();
    g = agopen(name, AGRAPH);
    GD_drawing(g) = NULL;
    return g;
}

Agraph_t *graphstrict(char *name)
{
    Agraph_t *g;

    if (!gvc)
	gvinit();
    g = agopen(name, AGRAPHSTRICT);
    GD_drawing(g) = NULL;
    return g;
}

Agraph_t *stringgraph(char *graphstring)
{
    Agraph_t *g;

    if (!gvc)
	gvinit();
    g = agmemread(graphstring);
    GD_drawing(g) = NULL;
    return g;
}

Agraph_t *readgraph(char *filename)
{
    FILE *f;
    Agraph_t *g;

    if (!gvc)
	gvinit();
    f = fopen(filename, "r");
    g = agread(f);
    fclose(f);
    GD_drawing(g) = NULL;
    return g;
}

/* create a subgraph of an existing graph */
Agraph_t *graph(Agraph_t *g, char *name)
{
    if (!gvc)
	return NULL;
    return agsubg(g, name);
}

Agnode_t *node(Agraph_t *g, char *name)
{
    if (!gvc)
	return NULL;
    return agnode(g, name);
}

Agedge_t *edge(Agnode_t *t, Agnode_t *h)
{
    return agedge(t->graph, t, h);
}

char *set(Agraph_t *g, char *attr, char *val)
{
    Agsym_t *a;

    a = agfindattr(g->root, attr);
    if (val) {
	if (!a)
	    a = agraphattr(g->root, attr, "");
	agxset(g, a->index, val);
    }
    else {
	if (a)
	    val = agxget(g, a->index);
	else
	    val = "";
    }
    return val;
}

char *set(Agraph_t *g, char *attr)
{
    Agsym_t *a;

    a = agfindattr(g->root, attr);
    if (a)
	return agxget(g, a->index);
    else
	return "";
}

char *set(Agnode_t *n, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    g = n->graph->root;
    a = agfindattr(g->proto->n, attr);
    if (val) {
        if (!a)
            a = agnodeattr(g, attr, "");
        agxset(n, a->index, val);
    }
    else {
        if (a)
            val = agxget(n, a->index);
        else
            val = "";
    }
    return val;
}

char *set(Agnode_t *n, char *attr)
{
    Agsym_t *a;

    a = agfindattr(n->graph->root->proto->n, attr);
    if (a)
        return agxget(n, a->index);
    else
        return "";
}

char *set(Agedge_t *e, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    g = e->tail->graph->root;
    a = agfindattr(g->proto->e, attr);
    if (val) {
        if (!a)
            a = agedgeattr(g, attr, "");
        agxset(e, a->index, val);
    }
    else {
        if (a)
            val = agxget(e, a->index);
        else
            val = "";
    }
    return val;
}

char *set(Agedge_t *e, char *attr)
{
    Agsym_t *a;

    a = agfindattr(e->tail->graph->root->proto->e, attr);
    if (a)
        return agxget(e, a->index);
    else
        return "";
}

Agnode_t *headof(Agedge_t *e)
{
    if (!e)
	return NULL;
    return e->head;
}

Agnode_t *tailof(Agedge_t *e)
{
    if (!e)
	return NULL;
    return e->tail;
}

Agraph_t *graphof(Agraph_t *g)
{
    if (!g || g == g->root)
	return NULL;
    return g->root;
}

Agraph_t *graphof(Agedge_t *e)
{
    if (!e)
	return NULL;
    return e->tail->graph;
}

Agraph_t *graphof(Agnode_t *n)
{
    if (!n)
	return NULL;
    return n->graph;
}

Agraph_t *rootof(Agraph_t *g)
{
    if (!g)
	return NULL;
    return g->root;
}

Agraph_t *firstsubg(Agraph_t *g)
{
    Agraph_t *mg;
    Agnode_t *n;
    Agedge_t *e;

    if (!g)
	return NULL;
    n = g->meta_node;
    if (!n) 
	return NULL;
    mg = n->graph;
    if (!mg) 
	return NULL;
    e = agfstout(mg, n);
    if (!e) 
	return NULL;
    return agusergraph(e->head);
}

Agraph_t *nextsubg(Agraph_t *g, Agraph_t *sg)
{
    Agraph_t *mg;
    Agnode_t *ng, *nsg;
    Agedge_t *e;

    if (!g || !sg)
	return NULL;
    ng = g->meta_node;
    nsg = sg->meta_node;
    if (!ng || !nsg) 
	return NULL;
    mg = ng->graph;
    if (!mg) 
	return NULL;
    e = agfindedge(mg, ng, nsg);
    if (!e) 
	return NULL;
    e = agnxtout(mg, e);
    if (!e) 
	return NULL;
    return agusergraph(e->head);
}

Agraph_t *firstsupg(Agraph_t *g)
{
    Agraph_t *mg;
    Agnode_t *n;
    Agedge_t *e;

    if (!g)
	return NULL;
    n = g->meta_node;
    if (!n) 
	return NULL;
    mg = n->graph;
    if (!mg) 
	return NULL;
    e = agfstin(mg, n);
    if (!e) 
	return NULL;
    return agusergraph(e->tail);
}

Agraph_t *nextsupg(Agraph_t *g, Agraph_t *sg)
{
    Agraph_t *mg;
    Agnode_t *ng, *nsg;
    Agedge_t *e;

    if (!g || !sg)
	return NULL;
    ng = g->meta_node;
    nsg = sg->meta_node;
    if (!ng || !nsg) 
	return NULL;
    mg = ng->graph;
    if (!mg) 
	return NULL;
    e = agfindedge(mg, nsg, ng);
    if (!e) 
	return NULL;
    e = agnxtin(mg, e);
    if (!e) 
	return NULL;
    return agusergraph(e->tail);
}

Agedge_t *firstout(Agraph_t *g)
{
    Agnode_t *n;

    if (!g)
	return NULL;
    n = agfstnode(g);
    if (!n)
	return NULL;
    return agfstout(g, n);
}

Agedge_t *nextout(Agraph_t *g, Agedge_t *e)
{
    Agnode_t *n;

    if (!g || !e)
	return NULL;
    e = agnxtout(g, e);
    if (e)
	return (e);
    n = agnxtnode(g, n);
    if (!n)
	return NULL;
    return agfstout(g, n);
}

Agedge_t *firstout(Agedge_t *e)
{ return NULL; }

Agedge_t *nextout(Agedge_t *e, Agedge_t *ee)
{ return NULL; }

Agedge_t *firstout(Agnode_t *n)
{
    if (!n)
	return NULL;
    return agfstout(n->graph, n);
}

Agedge_t *nextout(Agnode_t *n, Agedge_t *e)
{
    if (!n || !e)
	return NULL;
    return (agnxtout(n->graph, e));
}

Agedge_t *firstin(Agraph_t *g)
{
    Agnode_t *n;

    if (!g)
	return NULL;
    n = agfstnode(g);
    if (!n)
	return NULL;
    return(agfstin(g, n));
}

Agedge_t *nextin(Agraph_t *g, Agedge_t *e)
{
    Agnode_t *n;

    if (!g || !e)
	return NULL;
    e = agnxtin(g, e);
    if (e)
	return (e);
    n = agnxtnode(g, n);
    if (!n)
	return NULL;
    return (agfstin(g, n));
}

Agedge_t *firstin(Agedge_t *e)
{ return NULL; }

Agedge_t *nextin(Agedge_t *e, Agedge_t *ee)
{ return NULL; }

Agedge_t *firstin(Agnode_t *n)
{
    if (!n)
	return NULL;
    return(agfstin(n->graph, n));
}

Agedge_t *nextin(Agnode_t *n, Agedge_t *e)
{
    if (!n || !e)
	return NULL;
    return (agnxtin(n->graph, e));
}

Agnode_t *first(Agraph_t *g)
{
    return (agfstnode(g));
}

Agnode_t *next(Agraph_t *g, Agnode_t *n)
{
    return (agnxtnode(g, n));
}

Agnode_t *first(Agedge_t *e)
{
    if (!e)
	return NULL;
    return (e->tail);
}

Agnode_t *next(Agedge_t *e, Agnode_t *n)
{
    if (!e || n != e->tail)
	return NULL;
    return (e->head);
}

Agnode_t *first(Agnode_t *n)
{ return NULL; }

Agnode_t *next(Agnode_t *n, Agnode_t *nn)
{ return NULL; }

void rm(Agraph_t *g)
{
    Agedge_t *e;

    if (g->meta_node) {
	for (e = agfstout(g->meta_node->graph, g->meta_node); e;
			e = agnxtout(g->meta_node->graph, e)) {
	    rm(agusergraph(e->head));
        }
        if (g == g->root) {
            agclose(g);
        } else {
            agdelete(g->meta_node->graph, g->meta_node);
        }
    } else {
        fprintf(stderr, "subgraph has no meta_node\n");
    }
}

void rm(Agnode_t *n)
{
    agdelete(n->graph, n);
}

void rm(Agedge_t *e)
{
    agdelete(e->head->graph->root, e);
}

void layout(Agraph_t *g, char *engine)
{
    int err;

    err = gvLayout(gvc, g, engine);
}

void writegraph(Agraph_t *g, char *filename, char *format)
{
    int err;

    err = gvRenderFilename(gvc, g, format, filename);
}
