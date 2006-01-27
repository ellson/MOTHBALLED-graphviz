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

#include <string.h>
#include "gvc.h"

static GVC_t *gvc;
static char *empty_string = "";

Agraph_t *graph(char *name)
{
    if (!gvc)
	gvc = gvContext();
    return agopen(name, AGRAPH);
}

Agraph_t *digraph(char *name)
{
    if (!gvc)
	gvc = gvContext();
    return agopen(name, AGDIGRAPH);
}

Agraph_t *strictgraph(char *name)
{
    if (!gvc)
	gvc = gvContext();
    return agopen(name, AGRAPHSTRICT);
}

Agraph_t *strictdigraph(char *name)
{
    if (!gvc)
	gvc = gvContext();
    return agopen(name, AGDIGRAPHSTRICT);
}

Agraph_t *readstring(char *string)
{
    if (!gvc)
	gvc = gvContext();
    return agmemread(string);
}

Agraph_t *read(FILE *f)
{
    if (!gvc)
	gvc = gvContext();
    return agread(f);
}

Agraph_t *read(char *filename)
{
    FILE *f;
    Agraph_t *g;

    f = fopen(filename, "r");
    if (!f)
	return NULL;
    g = agread(f);
    fclose(f);
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
    // creating a protonode is not permitted
    if (!gvc || (name[0] == '\001' && strcmp (name, "\001proto") == 0))
	return NULL;
    return agnode(g, name);
}

Agedge_t *edge(Agnode_t *t, Agnode_t *h)
{
    // edges from/to the protonode are not permitted
    if (!gvc || !t || !h
      || (t->name[0] == '\001' && strcmp (t->name, "\001proto") == 0)
      || (h->name[0] == '\001' && strcmp (h->name, "\001proto") == 0))
	return NULL;
    return agedge(t->graph, t, h);
}

// induce tail if necessary
Agedge_t *edge(char *tname, Agnode_t *h)
{
    return edge(node(h->graph, tname), h);
}

// induce head if necessary
Agedge_t *edge(Agnode_t *t, char *hname)
{
    return edge(t, node(t->graph, hname));
}

// induce tail/head if necessary
Agedge_t *edge(Agraph_t *g, char *tname, char *hname)
{
    return edge(node(g, tname), node(g, hname));
}

//-------------------------------------------------
char *getv(Agraph_t *g, Agsym_t *a)
{
    char *val;

    if (!g || !a)
	return NULL;
    val = agxget(g, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *getv(Agraph_t *g, char *attr)
{
    Agsym_t *a;
    char *val;

    if (!g || !attr)
	return NULL;
    a = agfindattr(g->root, attr);
    if (!a)
	return empty_string;
    val = agxget(g, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *setv(Agraph_t *g, Agsym_t *a, char *val)
{
    if (!g || !a || !val)
	return NULL;
    agxset(g, a->index, val);
    return val;
}
char *setv(Agraph_t *g, char *attr, char *val)
{
    Agsym_t *a;

    if (!g || !attr || !val)
	return NULL;
    a = agfindattr(g->root, attr);
    if (!a)
        a = agraphattr(g->root, attr, empty_string);
    agxset(g, a->index, val);
    return val;
}
//-------------------------------------------------
char *getv(Agnode_t *n, Agsym_t *a)
{
    char *val;

    if (!n || !a)
	return NULL;
    val = agxget(n, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *getv(Agnode_t *n, char *attr)
{
    Agraph_t *g;
    Agsym_t *a;
    char *val;

    if (!n || !attr)
	return NULL;
    g = n->graph->root;
    a = agfindattr(g->proto->n, attr);
    if (!a)
	return empty_string;
    val = agxget(n, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *setv(Agnode_t *n, Agsym_t *a, char *val)
{
    if (!n || !a || !val)
	return NULL;
    agxset(n, a->index, val);
    return val;
}
char *setv(Agnode_t *n, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    if (!n || !attr || !val)
	return NULL;
    g = n->graph->root;
    a = agfindattr(g->proto->n, attr);
    if (!a)
        a = agnodeattr(g, attr, empty_string);
    agxset(n, a->index, val);
    return val;
}
//-------------------------------------------------
char *getv(Agedge_t *e, Agsym_t *a)
{
    char *val;

    if (!e || !a)
	return NULL;
    val =  agxget(e, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *getv(Agedge_t *e, char *attr)
{
    Agraph_t *g;
    Agsym_t *a;
    char *val;

    if (!e || !attr)
	return NULL;
    g = e->tail->graph;
    a = agfindattr(g->proto->e, attr);
    if (!a)
	return empty_string;
    val =  agxget(e, a->index);
    if (!val)
	return empty_string;
    return val;
}
char *setv(Agedge_t *e, Agsym_t *a, char *val)
{
    if (!e || !a || !val)
	return NULL;
    agxset(e, a->index, val);
    return val;
}
char *setv(Agedge_t *e, char *attr, char *val)
{
    Agraph_t *g;
    Agsym_t *a;

    if (!e || !attr || !val)
	return NULL;
    g = e->tail->graph->root;
    a = agfindattr(g->proto->e, attr);
    if (!a)
        a = agedgeattr(g, attr, empty_string);
    agxset(e, a->index, val);
    return val;
}
//-------------------------------------------------
Agraph_t *findsubg(Agraph_t *g, char *name)
{
    if (!g || !name)
	return NULL;
    return agfindsubg(g, name);
}

Agnode_t *findnode(Agraph_t *g, char *name)
{
    if (!g || !name)
	return NULL;
    return agfindnode(g, name);
}

Agedge_t *findedge(Agnode_t *t, Agnode_t *h)
{
    if (!t || !h)
	return NULL;
    return agfindedge(t->graph, t, h);
}

Agsym_t *findattr(Agraph_t *g, char *name)
{
    if (!g || !name)
	return NULL;
    return agfindattr(g, name);
}

Agsym_t *findattr(Agnode_t *n, char *name)
{
    if (!n || !name)
	return NULL;
    return agfindattr(n, name);
}

Agsym_t *findattr(Agedge_t *e, char *name)
{
    if (!e || !name)
	return NULL;
    return agfindattr(e, name);
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
Agnode_t *protonode(Agraph_t *g)
{
    if (!g)
        return NULL;
    return g->proto->n;
}

Agedge_t *protoedge(Agraph_t *g)
{
    if (!g)
        return NULL;
    return g->proto->e;
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
bool ok(Agraph_t *g)
{
    if (!g) 
	return false;
    return true;
}
bool ok(Agnode_t *n)
{
    if (!n) 
	return false;
    return true;
}
bool ok(Agedge_t *e)
{
    if (!e) 
	return false;
    return true;
}
bool ok(Agsym_t *a)
{
    if (!a) 
	return false;
    return true;
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
    Agedge_t *ne;

    if (!g || !e)
	return NULL;
    ne = agnxtout(g, e);
    if (ne)
	return (ne);
    n = agnxtnode(g, e->tail);
    if (!n)
	return NULL;
    return agfstout(g, n);
}

Agedge_t *firstedge(Agraph_t *g)
{
    if (!g)
	return NULL;
    return firstout(g);
} 

Agedge_t *nextedge(Agraph_t *g, Agedge_t *e)
{
    if (!g || !e)
	return NULL;
    return nextout(g, e);
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
    return agnxtout(n->graph, e);
}

Agnode_t *firsthead(Agnode_t *n)
{
    Agedge_t *e;

    if (!n)
	return NULL;
    e = agfstout(n->graph, n);
    if (!e)
	return NULL;
    return e->head;
}

Agnode_t *nexthead(Agnode_t *n, Agnode_t *h)
{
    Agedge_t *e;

    if (!n || !h)
	return NULL;
    e = agfindedge(n->graph, n, h);
    if (!e)
	return NULL;
    do {
	e = agnxtout(n->graph, e);
	if (!e)
	    return NULL;
    } while (e->head == h);
    return e->head;
}

Agedge_t *firstedge(Agnode_t *n)
{
    if (!n)
	return NULL;
    return agfstedge(n->graph, n);
} 

Agedge_t *nextedge(Agnode_t *n, Agedge_t *e)
{
    if (!n || !e)
	return NULL;
    return agnxtedge(n->graph, e, n); 
} 

Agedge_t *firstin(Agraph_t *g)
{
    Agnode_t *n;

    if (!g)
	return NULL;
    n = agfstnode(g);
    if (!n)
	return NULL;
    return agfstin(g, n);
}

Agedge_t *nextin(Agraph_t *g, Agedge_t *e)
{
    Agnode_t *n;
    Agedge_t *ne;

    if (!g || !e)
	return NULL;
    ne = agnxtin(g, e);
    if (ne)
	return (ne);
    n = agnxtnode(g, n);
    if (!n)
	return NULL;
    return agfstin(g, n);
}

Agedge_t *firstin(Agnode_t *n)
{
    if (!n)
	return NULL;
    return agfstin(n->graph, n);
}

Agedge_t *nextin(Agnode_t *n, Agedge_t *e)
{
    if (!n || !e)
	return NULL;
    return agnxtin(n->graph, e);
}

Agnode_t *firsttail(Agnode_t *n)
{
    Agedge_t *e;

    if (!n)
	return NULL;
    e = agfstin(n->graph, n);
    if (!e)
	return NULL;
    return e->tail;
}

Agnode_t *nexttail(Agnode_t *n, Agnode_t *t)
{
    Agedge_t *e;

    if (!n || !t)
	return NULL;
    e = agfindedge(n->graph, t, n);
    if (!e)
	return NULL;
    do {
	e = agnxtout(n->graph, e);
	if (!e)
	    return NULL;
    } while (e->tail == t);
    return e->tail;
}

Agnode_t *firstnode(Agraph_t *g)
{
    if (!g)
	return NULL;
    return agfstnode(g);
}

Agnode_t *nextnode(Agraph_t *g, Agnode_t *n)
{
    if (!g || !n)
	return NULL;
    return agnxtnode(g, n);
}

Agnode_t *firstnode(Agedge_t *e)
{
    if (!e)
	return NULL;
    return e->tail;
}

Agnode_t *nextnode(Agedge_t *e, Agnode_t *n)
{
    if (!e || n != e->tail)
	return NULL;
    return e->head;
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
    for (i = 0; i < dtsize(g->univ->globattr->dict); i++)
	if (a == g->univ->globattr->list[i])
	    break;
    i++;
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
    for (i = 0; i < dtsize(g->univ->nodeattr->dict); i++)
	if (a == g->univ->nodeattr->list[i])
	    break;
    i++;
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
    for (i = 0; i < dtsize(g->univ->edgeattr->dict); i++)
	if (a == g->univ->edgeattr->list[i])
	    break;
    i++;
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
    // removal of the protonode is not permitted
    if (n->name[0] == '\001' && strcmp (n->name, "\001proto") ==0)
	return;
    agdelete(n->graph, n);
}

void rm(Agedge_t *e)
{
    // removal of the protoedge is not permitted
    if ((e->head->name[0] == '\001' && strcmp (e->head->name, "\001proto") == 0)
     || (e->tail->name[0] == '\001' && strcmp (e->tail->name, "\001proto") == 0))
	return;
    agdelete(e->head->graph->root, e);
}

void layout(Agraph_t *g, char *engine)
{
    int err;

    err = gvLayout(gvc, g, engine);
}

// annotate the graph with layout information
void render(Agraph_t *g)
{
    attach_attrs(g);
}

// render to a filename
void render(Agraph_t *g, char *format, char *filename)
{
    int err;

    err = gvRenderFilename(gvc, g, format, filename);
}

// render to stdout
void render(Agraph_t *g, char *format)
{
    int err;

    err = gvRender(gvc, g, format, stdout);
}

// render to a FILE
void render(Agraph_t *g, char *format, FILE *f)
{
    int err;

    err = gvRender(gvc, g, format, f);
}

// FIXME - render to a caller provided memory blob
void render(Agraph_t *g, char *format, void **data)
{
//    FIXME
//
//    int err;
//
//    err = gvRenderData(gvc, g, format, data);
}
