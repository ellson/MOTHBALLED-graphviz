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
#include "libgraph.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

Agdict_t *agdictof(void *obj)
{
    Agdict_t *d = NULL;

    switch (TAG_OF(obj)) {
    case TAG_GRAPH:
	d = ((Agraph_t *) obj)->univ->globattr;
	break;
    case TAG_NODE:
	d = ((Agnode_t *) obj)->graph->univ->nodeattr;
	break;
    case TAG_EDGE:
	d = ((Agedge_t *) obj)->tail->graph->univ->edgeattr;
	break;
    }
    return d;
}

Agsym_t *agNEWsym(Agdict_t * dict, char *name, char *value)
{
    Agsym_t *a;
    int i;

    a = NEW(Agsym_t);
    a->name = agstrdup(name);
    a->value = agstrdup(value);
    a->printed = TRUE;
    i = a->index = dtsize(dict->dict);
    dict->list = ALLOC(i + 2, dict->list, Agsym_t *);
    dict->list[i++] = a;
    dict->list[i++] = NULL;
    dtinsert(dict->dict, a);
    return a;
}

static void obj_init_attr(void *obj, Agsym_t * attr)
{
    int i;
    Agraph_t *gobj;		/* generic object */

    gobj = (Agraph_t *) obj;
    i = attr->index;
    gobj->attr = ALLOC(i + 1, gobj->attr, char *);
    gobj->attr[i] = agstrdup(attr->value);
}

static void add_graph_attr(Agraph_t * g, Agsym_t * attr)
{
    Agnode_t *n;

    if (g->meta_node) {
	for (n = agfstnode(g->meta_node->graph); n;
	     n = agnxtnode(g->meta_node->graph, n))
	    obj_init_attr(agusergraph(n), attr);
    } else
	obj_init_attr(g, attr);
}

static void add_node_attr(Agraph_t * g, Agsym_t * attr)
{
    Agnode_t *n;
    Agproto_t *proto;

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	obj_init_attr(n, attr);
    if (g->meta_node) {
	for (n = agfstnode(g->meta_node->graph); n;
	     n = agnxtnode(g->meta_node->graph, n))
	    for (proto = agusergraph(n)->proto; proto; proto = proto->prev)
		obj_init_attr(proto->n, attr);
    } else
	for (proto = g->proto; proto; proto = proto->prev)
	    obj_init_attr(proto->n, attr);
}

static void add_edge_attr(Agraph_t * g, Agsym_t * attr)
{
    Agnode_t *n;
    Agedge_t *e;
    Agproto_t *proto;

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    obj_init_attr(e, attr);
    if (g->meta_node) {
	for (n = agfstnode(g->meta_node->graph); n;
	     n = agnxtnode(g->meta_node->graph, n))
	    for (proto = agusergraph(n)->proto; proto; proto = proto->prev)
		obj_init_attr(proto->e, attr);
    } else
	for (proto = g->proto; proto; proto = proto->prev)
	    obj_init_attr(proto->e, attr);
}

static Agsym_t *dcl_attr(void *obj, char *name, char *value)
{
    Agsym_t *rv;

    rv = agfindattr(obj, name);
    if (rv) {
	if (strcmp(rv->value, value)) {
	    agerr(AGWARN,
		  "Attribute %s=\"%s\" cannot be redeclared as \"%s\"\n",
		  name, rv->value, value);
	}
	return rv;
    }
    rv = agNEWsym(agdictof(obj), name, value);
    if (rv) {
	switch (TAG_OF(obj)) {
	case TAG_GRAPH:
	    add_graph_attr((Agraph_t *) obj, rv);
	    break;
	case TAG_NODE:
	    add_node_attr(((Agnode_t *) obj)->graph, rv);
	    break;
	case TAG_EDGE:
	    add_edge_attr(((Agedge_t *) obj)->head->graph, rv);
	    break;
	}
    }
    return rv;
}

Agraph_t *agprotograph()
{
    return AG.proto_g;
}

static void initproto(void)
{
    Agsym_t *a;
    Agraph_t *g;
    g = AG.proto_g = agopen("ProtoGraph", AGRAPH);
    a = dcl_attr(g->proto->e, KEY_ID, "");
    if (a->index != KEYX)
	abort();
    a = dcl_attr(g->proto->e, TAIL_ID, "");
    if (a->index != TAILX)
	abort();
    a->printed = FALSE;
    a = dcl_attr(g->proto->e, HEAD_ID, "");
    if (a->index != HEADX)
	abort();
    a->printed = FALSE;
}

Agsym_t *agraphattr(Agraph_t * g, char *name, char *value)
{
    if (g == NULL)
	g = AG.proto_g;
    if (g != g->root)
	return NULL;
    return dcl_attr(g, name, value);
}

Agsym_t *agnodeattr(Agraph_t * g, char *name, char *value)
{
    if (g == NULL)
	g = AG.proto_g;
    if (g != g->root)
	return NULL;
    return dcl_attr(g->proto->n, name, value);
}

Agsym_t *agedgeattr(Agraph_t * g, char *name, char *value)
{
    if (g == NULL)
	g = AG.proto_g;
    if (g != g->root)
	return NULL;
    return dcl_attr(g->proto->e, name, value);
}

/* attribute dictionaries */

static void agfreesym(void *ptr)
{
    Agsym_t *a;
    a = (Agsym_t *) ptr;
    agstrfree(a->name);
    agstrfree(a->value);
    free(a);
}

void agFREEdict(Agraph_t * g, Agdict_t * dict)
{
    int i;
    Agsym_t *a;

    g = g;
    dtclose(dict->dict);
    if (dict->list) {
	i = 0;
	while ((a = dict->list[i++]))
	    agfreesym(a);
	free(dict->list);
    }
    free(dict);
}

Agdict_t *agNEWdict(char *name)
{
    Agdict_t *dict;
    static Dtdisc_t symdisc = {
	offsetof(Agsym_t, name),	/* key */
	-1,			/* size */
	-1,			/* link */
	(Dtmake_f) 0,
	(Dtfree_f) 0,
	(Dtcompar_f) 0,		/* use strcmp */
	(Dthash_f) 0,
	(Dtmemory_f) 0,
	(Dtevent_f) 0
    };

    dict = NEW(Agdict_t);
    dict->name = name;
    dict->dict = dtopen(&symdisc, Dttree);
    dict->list = NULL;
    return dict;
}

void agcopydict(Agdict_t * to_dict, Agdict_t * from_dict)
{
    int i, n;
    Agsym_t *a, *b;

    n = dtsize(from_dict->dict);
    for (i = 0; i < n; i++) {
	a = from_dict->list[i];
	b = agNEWsym(to_dict, a->name, a->value);
	b->printed = a->printed;
	b->fixed = a->fixed;
#ifdef MSWIN32
	/* Microsoft C is a thing of wonder. */
	fprintf(stderr, "", a->name, a->value);
#endif
    }
}

Agsym_t *agfindattr(void *obj, char *name)
{
    Agsym_t *rv;
    Agdict_t *dict = agdictof(obj);

    rv = (Agsym_t *) dtmatch(dict->dict, name);
    return rv;
}

	/* this is normally called by the aginit() macro */
void aginitlib(int gs, int ns, int es)
{
    if (AG.proto_g == NULL) {
	AG.graph_nbytes = gs;
	AG.node_nbytes = ns;
	AG.edge_nbytes = es;
	AG.init_called = TRUE;
	initproto();
    } else
	if ((AG.graph_nbytes != gs) || (AG.node_nbytes != ns)
	    || (AG.edge_nbytes != es))
	agerr(AGWARN, "aginit() called multiply with inconsistent args\n");
}

char *agget(void *obj, char *attr)
{
    return agxget(obj, agindex(obj, attr));
}

int agset(void *obj, char *attr, char *value)
{
    return agxset(obj, agindex(obj, attr), value);
}

int agindex(void *obj, char *name)
{
    Agsym_t *a;
    int rv = -1;

    a = agfindattr(obj, name);
    if (a)
	rv = a->index;
    return rv;
}

char *agxget(void *obj, int index)
{
    if (index >= 0)
	return ((Agraph_t *) obj)->attr[index];
    return NULL;
}

int agxset(void *obj, int index, char *buf)
{
    char **p;
    if (index >= 0) {
	p = ((Agraph_t *) obj)->attr;
	agstrfree(p[index]);
	p[index] = agstrdup(buf);
	return 0;
    } else
	return -1;
}
