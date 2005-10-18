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

//-------------------------------------------------
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

//-------------------------------------------------
char *set(Agraph_t *g, Agsym_t *a)
{
    char *val;

    if (!g || !a)
	return NULL;
    val = agxget(g, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agraph_t *g, char *attr)
{
    Agsym_t *a;
    char *val;

    if (!g || !attr)
	return NULL;
    a = agfindattr(g->root, attr);
    if (!a)
	return "";
    val = agxget(g, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agraph_t *g, Agsym_t *a, char *val)
{
    if (!g || !a)
	return NULL;
    if (!val) {
	val = agxget(g, a->index);
    	if (!val)
		return "";
    	return val;
    }
    agxset(g, a->index, val);
    return val;
}
char *set(Agraph_t *g, char *attr, char *val)
{
    Agsym_t *a;

    if (!g || !attr)
	return NULL;
    a = agfindattr(g->root, attr);
    if (!a)
        a = agraphattr(g->root, attr, "");
    if (!val) {
	val = agxget(g, a->index);
    	if (!val)
	    return "";
    	return val;
    }
    agxset(g, a->index, val);
    return val;
}
//-------------------------------------------------
char *set(Agnode_t *n, Agsym_t *a)
{
    char *val;

    if (!n || !a)
	return NULL;
    val = agxget(n, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agnode_t *n, char *attr)
{
    Agraph_t *g;
    Agsym_t *a;
    char *val;

    if (!n || !attr)
	return NULL;
    g = n->graph->root;
    a = agfindattr(g->proto->n, attr);
    if (!a)
	return "";
    val = agxget(n, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agnode_t *n, Agsym_t *a, char *val)
{
    if (!n || !a)
	return NULL;
    if (!val) {
        val = agxget(n, a->index);
        if (!val)
	    return "";
        return val;
    }
    agxset(n, a->index, val);
    return val;
}
char *set(Agnode_t *n, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    if (!n || !attr)
	return NULL;
    g = n->graph->root;
    a = agfindattr(g->proto->n, attr);
    if (!a)
        a = agnodeattr(g, attr, "");
    if (!val) {
        val = agxget(n, a->index);
        if (!val)
	    return "";
        return val;
    }
    agxset(n, a->index, val);
    return val;
}
//-------------------------------------------------
char *set(Agedge_t *e, Agsym_t *a)
{
    char *val;

    if (!e || !a)
	return NULL;
    val =  agxget(e, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agedge_t *e, char *attr)
{
    Agraph_t *g;
    Agsym_t *a;
    char *val;

    if (!e || !attr)
	return NULL;
    g = e->head->graph->root;
    a = agfindattr(g->proto->e, attr);
    if (!a)
	return "";
    val =  agxget(e, a->index);
    if (!val)
	return "";
    return val;
}
char *set(Agedge_t *e, Agsym_t *a, char *val)
{
    if (!e || !a)
	return NULL;
    if (!val) {
        val = agxget(e, a->index);
        if (!val)
	    return "";
        return val;
    }
    agxset(e, a->index, val);
    return val;
}
char *set(Agedge_t *e, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    if (!e || !attr)
	return NULL;
    g = e->head->graph->root;
    a = agfindattr(g->proto->e, attr);
    if (!a)
        a = agnodeattr(g, attr, "");
    if (!val) {
        val = agxget(e, a->index);
        if (!val)
	    return "";
        return val;
    }
    agxset(e, a->index, val);
    return val;
}
//-------------------------------------------------

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

//-------------------------------------------------
char *nameof(Agraph_t *g)
{
    if (!g)
	return NULL;
    return g->name;
}
char *nameof(Agnode_t *n)
{
    if (!n)
	return NULL;
    return n->name;
}
//char *nameof(Agedge_t *e)
//{
//    if (!e)
//	return NULL;
//    return e->name;
//}
char *nameof(Agsym_t *a)
{
    if (!a)
	return NULL;
    return a->name;
}

//-------------------------------------------------
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

Agnode_t *firstnode(Agraph_t *g)
{
    return (agfstnode(g));
}

Agnode_t *nextnode(Agraph_t *g, Agnode_t *n)
{
    return (agnxtnode(g, n));
}

Agnode_t *firstnode(Agedge_t *e)
{
    if (!e)
	return NULL;
    return (e->tail);
}

Agnode_t *nextnode(Agedge_t *e, Agnode_t *n)
{
    if (!e || n != e->tail)
	return NULL;
    return (e->head);
}

Agsym_t *firstattr(Agraph_t *g)
{
    if (!g)
	return NULL;
    g = g->root;
    if (dtsize(g->univ->globattr->dict) == 0)
	return NULL;
    return g->univ->globattr->list[0];
}

Agsym_t *nextattr(Agraph_t *g, Agsym_t *a)
{
    int i;

    if (!g || !a)
        return NULL;
    g = g->root;
    i = a->index + 1;
    if (i > dtsize(g->univ->globattr->dict))
        return NULL;
    return g->univ->globattr->list[i];
}

Agsym_t *firstattr(Agnode_t *n)
{
    Agraph_t *g;

    if (!n)
	return NULL;
    g = n->graph;
    if (dtsize(g->univ->nodeattr->dict) == 0)
	return NULL;
    return g->univ->nodeattr->list[0];
}

Agsym_t *nextattr(Agnode_t *n, Agsym_t *a)
{
    Agraph_t *g;
    int i;

    if (!n || !a)
        return NULL;
    g = n->graph;
    i = a->index + 1;
    if (i > dtsize(g->univ->nodeattr->dict))
        return NULL;
    return g->univ->nodeattr->list[i];
}

Agsym_t *firstattr(Agedge_t *e)
{
    Agraph_t *g;

    if (!e)
	return NULL;
    g = e->tail->graph;
    if (dtsize(g->univ->edgeattr->dict) == 0)
	return NULL;
    return g->univ->edgeattr->list[0];
}

Agsym_t *nextattr(Agedge_t *e, Agsym_t *a)
{
    Agraph_t *g;
    int i;

    if (!e || !a)
        return NULL;
    g = e->tail->graph;
    i = a->index + 1;
    if (i > dtsize(g->univ->edgeattr->dict))
        return NULL;
    return g->univ->edgeattr->list[i];
}

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
