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

#include "gvplugin.h"
#include "types.h"
#include "graph.h"
#include "gvcint.h"
#include "gvcproc.h"

#include <string.h>
extern char *strdup_and_subst_graph(char *str, Agraph_t * g);
extern char *strdup_and_subst_edge(char *str, Agedge_t * e);
extern char *strdup_and_subst_node(char *str, Agnode_t * n);
extern void emit_graph(GVJ_t * job, graph_t * g);
extern bool overlap_edge(edge_t *e, boxf b);
extern bool overlap_node(node_t *n, boxf b);
extern int gvLayout(GVC_t *gvc, graph_t *g, char *engine);
extern int gvRenderFilename(GVC_t *gvc, graph_t *g, char *format, char *filename);

#define PANFACTOR 10
#define ZOOMFACTOR 1.1
#define EPSILON .0001

static char *s_digraph = "digraph";
static char *s_graph = "graph";
static char *s_subgraph = "subgraph";
static char *s_node = "node";
static char *s_edge = "edge";
static char *s_tooltip = "tooltip";
static char *s_href = "href";
static char *s_URL = "URL";
static char *s_tailport = "tailport";
static char *s_headport = "headport";
static char *s_key = "key";

static void gv_graph_state(GVJ_t *job, graph_t *g)
{
    int i, j;
    Agsym_t *a;
    gv_argvlist_t *list;

    list = &(job->selected_obj_type_name);
    j = 0;
    if (g == g->root) {
	if (g->kind && AGFLAG_DIRECTED) 
            gv_argvlist_set_item(list, j++, s_digraph);
	else
            gv_argvlist_set_item(list, j++, s_graph);
    }
    else {
        gv_argvlist_set_item(list, j++, s_subgraph);
    }
    gv_argvlist_set_item(list, j++, g->name);
    list->argc = j;

    list = &(job->selected_obj_attributes);
    for (i = 0, j = 0; i < dtsize(g->univ->globattr->dict); i++) {
        a = g->univ->globattr->list[i];
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, agxget(g, a->index));
    }
    list->argc = j;

    a = agfindattr(g->root, s_href);
    if (!a)
	a = agfindattr(g->root, s_URL);
    if (a)
	job->selected_href = strdup_and_subst_graph(agxget(g, a->index), g);
}

static void gv_node_state(GVJ_t *job, node_t *n)
{
    int i, j;
    Agsym_t *a;
    Agraph_t *g;
    gv_argvlist_t *list;

    list = &(job->selected_obj_type_name);
    j = 0;
    gv_argvlist_set_item(list, j++, s_node);
    gv_argvlist_set_item(list, j++, n->name);
    list->argc = j;

    list = &(job->selected_obj_attributes);
    g = n -> graph -> root;
    for (i = 0, j = 0; i < dtsize(g->univ->nodeattr->dict); i++) {
        a = g->univ->nodeattr->list[i];
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, agxget(n, a->index));
    }
    list->argc = j;

    a = agfindattr(n->graph->proto->n, s_href);
    if (!a)
        a = agfindattr(n->graph->proto->n, s_URL);
    if (a)
	job->selected_href = strdup_and_subst_node(agxget(n, a->index), n);
}

static void gv_edge_state(GVJ_t *job, edge_t *e)
{
    int i, j;
    Agsym_t *a;
    Agraph_t *g;
    gv_argvlist_t *nlist, *alist;

    nlist = &(job->selected_obj_type_name);

    /* only tail, head, and key are strictly identifying properties,
     * but we commonly alse use edge kind (e.g. "->") and tailport,headport
     * in edge names */
    j = 0;
    gv_argvlist_set_item(nlist, j++, s_edge);
    gv_argvlist_set_item(nlist, j++, e->tail->name);
    j++; /* skip tailport slot for now */
    gv_argvlist_set_item(nlist, j++, (e->tail->graph->kind && AGFLAG_DIRECTED)?"->":"--");
    gv_argvlist_set_item(nlist, j++, e->head->name);
    j++; /* skip headport slot for now */
    j++; /* skip key slot for now */
    nlist->argc = j;

    alist = &(job->selected_obj_attributes);
    g = e -> head -> graph -> root;
    for (i = 0, j = 0; i < dtsize(g->univ->edgeattr->dict); i++) {
        a = g->univ->edgeattr->list[i];

	/* tailport and headport can be shown as part of the name, but they
	 * are not identifying properties of the edge so we 
	 * also list them as modifyable attributes. */
        if (strcmp(a->name,s_tailport) == 0)
	    gv_argvlist_set_item(nlist, 2, agxget(e, a->index));
	else if (strcmp(a->name,s_headport) == 0)
	    gv_argvlist_set_item(nlist, 5, agxget(e, a->index));

	/* key is strictly an identifying property to distinguish multiple
	 * edges between the same node pair.   Its non-writable, so
	 * no need to list it as an attribute as well. */
	else if (strcmp(a->name,s_key) == 0) {
	    gv_argvlist_set_item(nlist, 6, agxget(e, a->index));
	    continue;
	}

        gv_argvlist_set_item(alist, j++, a->name);
        gv_argvlist_set_item(alist, j++, agxget(e, a->index));
    }
    alist->argc = j;

    a = agfindattr(e->head->graph->proto->e, s_href);
    if (!a)
	a = agfindattr(e->head->graph->proto->e, s_URL);
    if (a)
	job->selected_href = strdup_and_subst_edge(agxget(e, a->index), e);
}

static void gvevent_refresh(GVJ_t * job)
{
    graph_t *g = job->gvc->g;

    if (!job->selected_obj) {
	job->selected_obj = g;
	GD_selected(g) = true;
	gv_graph_state(job, g);
    }
    emit_graph(job, g);
}

/* recursively find innermost cluster containing the point */
static graph_t *gvevent_find_cluster(graph_t *g, boxf b)
{
    int i;
    graph_t *sg;
    boxf bb;

    for (i = 1; i <= GD_n_cluster(g); i++) {
	sg = gvevent_find_cluster(GD_clust(g)[i], b);
	if (sg)
	    return(sg);
    }
    B2BF(GD_bb(g), bb);
    if (OVERLAP(b, bb))
	return g;
    return NULL;
}

static void * gvevent_find_obj(graph_t *g, boxf b)
{
    graph_t *sg;
    node_t *n;
    edge_t *e;

    /* edges might overlap nodes, so search them first */
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    if (overlap_edge(e, b))
	        return (void *)e;
    /* search graph backwards to get topmost node, in case of overlap */
    for (n = aglstnode(g); n; n = agprvnode(g, n))
	if (overlap_node(n, b))
	    return (void *)n;
    /* search for innermost cluster */
    sg = gvevent_find_cluster(g, b);
    if (sg)
	return (void *)sg;

    /* otherwise - we're always in the graph */
    return (void *)g;
}

static void gvevent_leave_obj(GVJ_t * job)
{
    void *obj = job->current_obj;

    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    GD_active((graph_t*)obj) = false;
	    break;
        case AGNODE:
	    ND_active((node_t*)obj) = false;
	    break;
        case AGEDGE:
	    ED_active((edge_t*)obj) = false;
	    break;
        }
    }
    job->active_tooltip = NULL;
}

static void gvevent_enter_obj(GVJ_t * job)
{
    void *obj;
    graph_t *g;
    edge_t *e;
    node_t *n;
    Agsym_t *a;

    if (job->active_tooltip) {
	free(job->active_tooltip);
	job->active_tooltip = NULL;
    }
    obj = job->current_obj;
    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    g = (graph_t*)obj;
	    GD_active(g) = true;
	    a = agfindattr(g->root, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_graph(agxget(g, a->index), g);
	    break;
        case AGNODE:
	    n = (node_t*)obj;
	    ND_active(n) = true;
	    a = agfindattr(n->graph->proto->n, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_node(agxget(n, a->index), n);
	    break;
        case AGEDGE:
	    e = (edge_t*)obj;
	    ED_active(e) = true;
	    a = agfindattr(e->head->graph->proto->e, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_edge(agxget(e, a->index), e);
	    break;
        }
    }
}

/* CLOSEENOUGH is in window units - probably should be a feature... */
#define CLOSEENOUGH 1

static void gvevent_find_current_obj(GVJ_t * job, pointf pointer)
{
    void *obj;
    pointf p;
    boxf b;
    double closeenough;

    /* convert window point to graph coordinates */
    if (job->rotation) {
	p.x = job->focus.y - (pointer.y - job->height / 2.) / job->compscale.x;
	p.y = job->focus.x + (pointer.x - job->width / 2.) / job->compscale.y;
    }
    else {
	p.x = job->focus.x + (pointer.x - job->width / 2.) / job->compscale.x;
	p.y = job->focus.y + (pointer.y - job->height / 2.) / job->compscale.y;
    }
    closeenough = CLOSEENOUGH / job->compscale.x;

    b.UR.x = p.x + closeenough;
    b.UR.y = p.y + closeenough;
    b.LL.x = p.x - closeenough;
    b.LL.y = p.y - closeenough;

    obj = gvevent_find_obj(job->gvc->g, b);
    if (obj != job->current_obj) {
	gvevent_leave_obj(job);
	job->current_obj = obj;
	gvevent_enter_obj(job);
	job->needs_refresh = 1;
    }
}

static void gvevent_select_current_obj(GVJ_t * job)
{
    void *obj;

    obj = job->selected_obj;
    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    GD_visited((graph_t*)obj) = true;
	    GD_selected((graph_t*)obj) = false;
	    break;
        case AGNODE:
	    ND_visited((node_t*)obj) = true;
	    ND_selected((node_t*)obj) = false;
	    break;
        case AGEDGE:
	    ED_visited((edge_t*)obj) = true;
	    ED_selected((edge_t*)obj) = false;
	    break;
        }
    }

    if (job->selected_href) {
	free(job->selected_href);
        job->selected_href = NULL;
    }

    obj = job->selected_obj = job->current_obj;
    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    GD_selected((graph_t*)obj) = true;
	    gv_graph_state(job, (graph_t*)obj);
	    break;
        case AGNODE:
	    ND_selected((node_t*)obj) = true;
	    gv_node_state(job, (node_t*)obj);
	    break;
        case AGEDGE:
	    ED_selected((edge_t*)obj) = true;
	    gv_edge_state(job, (edge_t*)obj);
	    break;
        }
    }

#if 0
for (i = 0; i < job->selected_obj_type_name.argc; i++)
    fprintf(stderr,"%s%s", job->selected_obj_type_name.argv[i],
	(i==(job->selected_obj_type_name.argc - 1))?"\n":" ");
for (i = 0; i < job->selected_obj_attributes.argc; i++)
    fprintf(stderr,"%s%s", job->selected_obj_attributes.argv[i], (i%2)?"\n":" = ");
fprintf(stderr,"\n");
#endif
}

static void gvevent_button_press(GVJ_t * job, int button, pointf pointer)
{
    switch (button) {
    case 1: /* select / create in edit mode */
	gvevent_find_current_obj(job, pointer);
	gvevent_select_current_obj(job);
        job->click = 1;
	job->active = button;
	job->needs_refresh = 1;
	break;
    case 2: /* pan */
        job->click = 1;
	job->active = button;
	job->needs_refresh = 1;
	break;
    case 3: /* insert node or edge */
	gvevent_find_current_obj(job, pointer);
        job->click = 1;
	job->active = button;
	job->needs_refresh = 1;
	break;
    case 4:
	/* scrollwheel zoom in at current mouse x,y */
	job->fit_mode = 0;
	job->focus.x +=  (pointer.x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y += -(pointer.y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->zoom *= ZOOMFACTOR;
	job->needs_refresh = 1;
	break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
	job->fit_mode = 0;
	job->zoom /= ZOOMFACTOR;
	job->focus.x -=  (pointer.x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y -= -(pointer.y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->needs_refresh = 1;
	break;
    }
    job->oldpointer = pointer;
}

static void gvevent_button_release(GVJ_t *job, int button, pointf pointer)
{
    job->click = 0;
    job->active = 0;
}

static void gvevent_motion(GVJ_t * job, pointf pointer)
{
    double dx = pointer.x - job->oldpointer.x;
    double dy = pointer.y - job->oldpointer.y;

    if (abs(dx) < EPSILON && abs(dy) < EPSILON)  /* ignore motion events with no motion */
	return;

    switch (job->active) {
    case 0: /* drag with no button - */
	gvevent_find_current_obj(job, pointer);
	break;
    case 1: /* drag with button 1 - drag object */
	/* FIXME - to be implemented */
	break;
    case 2: /* drag with button 2 - pan graph */
	job->focus.x -=  dx / job->zoom;
	job->focus.y -= -dy / job->zoom;
	job->needs_refresh = 1;
	break;
    case 3: /* drag with button 3 - drag inserted node or uncompleted edge */
	break;
    }
    job->oldpointer = pointer;
}

static int quit_cb(GVJ_t * job)
{
    return 1;
}

static int left_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.x += PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int right_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.x -= PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int up_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.y += -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int down_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.y -= -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int zoom_in_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->zoom *= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int zoom_out_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->zoom /= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int toggle_fit_cb(GVJ_t * job)
{
    job->fit_mode = !job->fit_mode;
    if (job->fit_mode) {
	int dflt_width, dflt_height;
	dflt_width = job->width;
	dflt_height = job->height;
	job->zoom =
	    MIN((double) job->width / (double) dflt_width,
		(double) job->height / (double) dflt_height);
	job->focus.x = 0.0;
	job->focus.y = 0.0;
	job->needs_refresh = 1;
    }
    return 0;
}

static void gvevent_modify (GVJ_t * job, char *name, char *value)
{
    /* FIXME */
}

static void gvevent_delete (GVJ_t * job)
{
    /* FIXME */
}

static void gvevent_read (GVJ_t * job, char *filename, char *layout)
{
    FILE *f;

    if (job->gvc->g) {
        gvFreeLayout(job->gvc, job->gvc->g);
	agclose(job->gvc->g);
    }
    f = fopen(filename, "r");
    if (!f)
	return;   /* FIXME - need some error handling */
    job->gvc->g = agread(f);
    fclose(f);
    gvLayout(job->gvc, job->gvc->g, layout);
}

static void gvevent_layout (GVJ_t * job, char *layout)
{
    gvLayout(job->gvc, job->gvc->g, layout);
}

static void gvevent_render (GVJ_t * job, char *format, char *filename)
{
    gvRenderFilename(job->gvc, job->gvc->g, format, filename);
}


gvevent_key_binding_t gvevent_key_binding[] = {
    {"Q", quit_cb},
    {"Left", left_cb},
    {"KP_Left", left_cb},
    {"Right", right_cb},
    {"KP_Right", right_cb},
    {"Up", up_cb},
    {"KP_Up", up_cb},
    {"Down", down_cb},
    {"KP_Down", down_cb},
    {"plus", zoom_in_cb},
    {"KP_Add", zoom_in_cb},
    {"minus", zoom_out_cb},
    {"KP_Subtract", zoom_out_cb},
    {"F", toggle_fit_cb},
};

int gvevent_key_binding_size = ARRAY_SIZE(gvevent_key_binding);

gvdevice_callbacks_t gvdevice_callbacks = {
    gvevent_refresh,
    gvevent_button_press,
    gvevent_button_release,
    gvevent_motion,
    gvevent_modify,
    gvevent_delete,
    gvevent_read,
    gvevent_layout,
    gvevent_render,
};
