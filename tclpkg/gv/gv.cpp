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

char *Info[] = {
    "gv",
    VERSION,
    BUILDDATE
};

static void gvinit()
{
    aginit();
    agnodeattr(NULL, "label", NODENAME_ESC);
    gvc = gvNEWcontext(Info, username());

    /* configure for available plugins and codegens */
    gvconfig(gvc, FALSE);
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

static void reset_layout(Agraph_t *g)
{
    g = g->root;
    if (GD_drawing(g)) {        /* only cleanup once between layouts */
	gvFreeLayout(gvc, g);
        GD_drawing(g) = NULL;
    }
}

void layout(Agraph_t *g, char *engine)
{
    int err;

    err = gvLayout(gvc, g, engine);
    if (err) {
	fprintf(stderr, "Layout type: \"%s\" not recognized. Use one of:%s\n",                engine, gvplugin_list(gvc, API_layout, engine));
	return;
    }
}

void writegraph(Agraph_t *g, char *filename, char *format)
{
    int err;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    err = gvrender_output_langname_job(gvc, format);
    if (err == NO_SUPPORT) {
        fprintf(stderr, "Renderer type: \"%s\" not recognized. Use one of:%s\n",
		format, gvplugin_list(gvc, API_render, format));
        return;
    }

    job = gvc->job;
    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && job->output_lang != CANONICAL_DOT) {
        fprintf(stderr, "Layout was not done\n");
        return;
    }
    gvrender_output_filename_job(gvc, filename);
    
    gvRenderJobs(gvc, g);
    gvrender_delete_jobs(gvc);
}
