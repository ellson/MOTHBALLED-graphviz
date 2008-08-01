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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "gvplugin_layout.h"

#ifdef WITH_CGRAPH
#include <cgraph.h>
#else
#include <graph.h>
#endif

#include "gvcint.h"
#include "gvcproc.h"

extern char *strdup_and_subst_obj(char *str, void * n);
extern void emit_graph(GVJ_t * job, graph_t * g);
extern boolean overlap_edge(edge_t *e, boxf b);
extern boolean overlap_node(node_t *n, boxf b);
extern int gvLayout(GVC_t *gvc, graph_t *g, char *engine);
extern int gvRenderFilename(GVC_t *gvc, graph_t *g, char *format, char *filename);
extern void graph_cleanup(graph_t *g);

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
    Agsym_t *a=NULL;
    gv_argvlist_t *list;
	list = &(job->selected_obj_type_name);
    j = 0;

	
#ifdef WITH_CGRAPH
	if (g == agroot(g)) {
#else
	if (g == g->root) {
#endif

#ifdef WITH_CGRAPH
	if (AGTYPE(g) && agisdirected(g)) 
#else
	if (g->kind && AGFLAG_DIRECTED) 
#endif


		gv_argvlist_set_item(list, j++, s_digraph);
	else
            gv_argvlist_set_item(list, j++, s_graph);
    }
    else {
        gv_argvlist_set_item(list, j++, s_subgraph);
    }

#ifdef WITH_CGRAPH
	gv_argvlist_set_item(list, j++, agname(g));
#else
	gv_argvlist_set_item(list, j++, g->name);
#endif
	
    list->argc = j;

    list = &(job->selected_obj_attributes);
	

#ifdef WITH_CGRAPH
	j=0;
	while (a = agnxtattr(g, AGRAPH, a)) {
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, a->defval);  /* For graphs, defval and the graph's value are the same */
        gv_argvlist_set_item(list, j++, (char*)GVATTR_STRING);
    }
#else
	for (i = 0, j = 0; i < dtsize(g->univ->globattr->dict); i++) {
        a = g->univ->globattr->list[i];
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, agxget(g, a->index));
        gv_argvlist_set_item(list, j++, (char*)GVATTR_STRING);
    }
#endif

	list->argc = j;


#ifdef WITH_CGRAPH
	a = agfindattr(agroot(g), s_href);
    if (!a)
	a = agfindattr(agroot(g), s_URL);

#else
	a = agfindattr(g->root, s_href);
    if (!a)
	a = agfindattr(g->root, s_URL);
#endif
	
	
    if (a)

#ifdef WITH_CGRAPH
		job->selected_href = strdup_and_subst_obj(agxget(g, a), (void*)g);
#else
		job->selected_href = strdup_and_subst_obj(agxget(g, a->index), (void*)g);
#endif
		
		
}

/*
 * Summary   :      
 * Parameters: *GVJ_s,*Agnode_s 
 * Return    : no return value
 */
static void gv_node_state(GVJ_t *job, node_t *n)
{
    int i, j;
    Agsym_t *a;
    Agraph_t *g;
    gv_argvlist_t *list;

    list = &(job->selected_obj_type_name);
    j = 0;
    gv_argvlist_set_item(list, j++, s_node);

#ifdef WITH_CGRAPH
	gv_argvlist_set_item(list, j++, agnameof(n));
    list->argc = j;
    list = &(job->selected_obj_attributes);

	g= agroot(agraphof(n));
	j=0;
	while (a = agnxtattr(g, AGNODE, a)) {
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, a->defval);  /* For graphs, defval and the graph's value are the same */
        gv_argvlist_set_item(list, j++, (char*)GVATTR_STRING);
    }
    list->argc = j;

	/*ASK ERG!!!!!!!!!!!!!!!!!!!!!!
	/*Agproto_t *proto;
	a = agfindattr(agraphof(n)->proto->n, s_href);
    if (!a)
        a = agfindattr(n->graph->proto->n, s_URL);
    if (a)
	job->selected_href = strdup_and_subst_obj(agxget(n, a), (void*)n);*/
#else
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
	job->selected_href = strdup_and_subst_obj(agxget(n, a->index), (void*)n);

#endif

}

static void gv_edge_state(GVJ_t *job, edge_t *e)
{
#ifdef WITH_CGRAPH
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
	job->selected_href = strdup_and_subst_obj(agxget(e, a->index), (void*)e);

#else
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
	job->selected_href = strdup_and_subst_obj(agxget(e, a->index), (void*)e);
#endif
}

static void gvevent_refresh(GVJ_t * job)
{
    graph_t *g = job->gvc->g;

    if (!job->selected_obj) {
	job->selected_obj = g;
	GD_gui_state(g) |= GUI_STATE_SELECTED;
	gv_graph_state(job, g);
    }
    emit_graph(job, g);
    job->has_been_rendered = TRUE;
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
	    GD_gui_state((graph_t*)obj) &= ~GUI_STATE_ACTIVE;
	    break;
        case AGNODE:
	    ND_gui_state((node_t*)obj) &= ~GUI_STATE_ACTIVE;
	    break;
        case AGEDGE:
	    ED_gui_state((edge_t*)obj) &= ~GUI_STATE_ACTIVE;
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
	    GD_gui_state(g) |= GUI_STATE_ACTIVE;
	    a = agfindattr(g->root, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_obj(agxget(g, a->index), obj);
	    break;
        case AGNODE:
	    n = (node_t*)obj;
	    ND_gui_state(n) |= GUI_STATE_ACTIVE;
	    a = agfindattr(n->graph->proto->n, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_obj(agxget(n, a->index), obj);
	    break;
        case AGEDGE:
	    e = (edge_t*)obj;
	    ED_gui_state(e) |= GUI_STATE_ACTIVE;
	    a = agfindattr(e->head->graph->proto->e, s_tooltip);
	    if (a)
		job->active_tooltip = strdup_and_subst_obj(agxget(e, a->index), obj);
	    break;
        }
    }
}

static pointf pointer2graph (GVJ_t *job, pointf pointer)
{
    pointf p;

    /* transform position in device units to position in graph units */
    if (job->rotation) {
        p.x = pointer.y / (job->zoom * job->devscale.y) - job->translation.x;
        p.y = -pointer.x / (job->zoom * job->devscale.x) - job->translation.y;
    }
    else {
        p.x = pointer.x / (job->zoom * job->devscale.x) - job->translation.x;
        p.y = pointer.y / (job->zoom * job->devscale.y) - job->translation.y;
    }
    return p;
}

/* CLOSEENOUGH is in 1/72 - probably should be a feature... */
#define CLOSEENOUGH 1

static void gvevent_find_current_obj(GVJ_t * job, pointf pointer)
{
    void *obj;
    boxf b;
    double closeenough;
    pointf p;

    p =  pointer2graph (job, pointer);

    /* convert window point to graph coordinates */
    closeenough = CLOSEENOUGH / job->zoom;

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
	    GD_gui_state((graph_t*)obj) |= GUI_STATE_VISITED;
	    GD_gui_state((graph_t*)obj) &= ~GUI_STATE_SELECTED;
	    break;
        case AGNODE:
	    ND_gui_state((node_t*)obj) |= GUI_STATE_VISITED;
	    ND_gui_state((node_t*)obj) &= ~GUI_STATE_SELECTED;
	    break;
        case AGEDGE:
	    ED_gui_state((edge_t*)obj) |= GUI_STATE_VISITED;
	    ED_gui_state((edge_t*)obj) &= ~GUI_STATE_SELECTED;
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
	    GD_gui_state((graph_t*)obj) |= GUI_STATE_SELECTED;
	    gv_graph_state(job, (graph_t*)obj);
	    break;
        case AGNODE:
	    ND_gui_state((node_t*)obj) |= GUI_STATE_SELECTED;
	    gv_node_state(job, (node_t*)obj);
	    break;
        case AGEDGE:
	    ED_gui_state((edge_t*)obj) |= GUI_STATE_SELECTED;
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
	job->button = button;
	job->needs_refresh = 1;
	break;
    case 2: /* pan */
        job->click = 1;
	job->button = button;
	job->needs_refresh = 1;
	break;
    case 3: /* insert node or edge */
	gvevent_find_current_obj(job, pointer);
        job->click = 1;
	job->button = button;
	job->needs_refresh = 1;
	break;
    case 4:
        /* scrollwheel zoom in at current mouse x,y */
/* FIXME - should code window 0,0 point as feature with Y_GOES_DOWN */
        job->fit_mode = 0;
        if (job->rotation) {
            job->focus.x -= (pointer.y - job->height)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.y);
            job->focus.y += (pointer.x)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.x);
        }
        else {
            job->focus.x += (pointer.x)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.x);
            job->focus.y += (pointer.y - job->height)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.y);
        }
        job->zoom *= ZOOMFACTOR;
        job->needs_refresh = 1;
        break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
        job->fit_mode = 0;
        job->zoom /= ZOOMFACTOR;
        if (job->rotation) {
            job->focus.x += (pointer.y - job->height)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.y);
            job->focus.y -= (pointer.x)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.x);
        }
        else {
            job->focus.x -= (pointer.x)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.x);
            job->focus.y -= (pointer.y - job->height)
                    * (ZOOMFACTOR - 1.) / (job->zoom * job->devscale.y);
        }
        job->needs_refresh = 1;
        break;
    }
    job->oldpointer = pointer;
}

static void gvevent_button_release(GVJ_t *job, int button, pointf pointer)
{
    job->click = 0;
    job->button = 0;
}

static void gvevent_motion(GVJ_t * job, pointf pointer)
{
    /* dx,dy change in position, in device independent points */
    double dx = (pointer.x - job->oldpointer.x) / job->devscale.x;
    double dy = (pointer.y - job->oldpointer.y) / job->devscale.y;

    if (abs(dx) < EPSILON && abs(dy) < EPSILON)  /* ignore motion events with no motion */
	return;

    switch (job->button) {
    case 0: /* drag with no button - */
	gvevent_find_current_obj(job, pointer);
	break;
    case 1: /* drag with button 1 - drag object */
	/* FIXME - to be implemented */
	break;
    case 2: /* drag with button 2 - pan graph */
	if (job->rotation) {
	    job->focus.x -= dy / job->zoom;
	    job->focus.y += dx / job->zoom;
	}
	else {
	    job->focus.x -= dx / job->zoom;
	    job->focus.y -= dy / job->zoom;
	}
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
/*FIXME - should allow for margins */
/*      - similar zoom_to_fit code exists in: */
/*      plugin/gtk/callbacks.c */
/*      plugin/xlib/gvdevice_xlib.c */
/*      lib/gvc/gvevent.c */

    job->fit_mode = !job->fit_mode;
    if (job->fit_mode) {
	/* FIXME - this code looks wrong */
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
    GVC_t *gvc;
    Agraph_t *g = NULL;
    gvlayout_engine_t *gvle;

    gvc = job->gvc;
    if (!filename) {
	g = agopen("G", AGDIGRAPH);
	job->output_filename = "new.dot";
    }
    else {
	f = fopen(filename, "r");
	if (!f)
	   return;   /* FIXME - need some error handling */
	g = agread(f);
	fclose(f);
    }
    if (!g)
	return;   /* FIXME - need some error handling */
    if (gvc->g) {
	gvle = gvc->layout.engine;
	if (gvle && gvle->cleanup)
	    gvle->cleanup(gvc->g);
	graph_cleanup(gvc->g);
	agclose(gvc->g);
    }
    gvc->g = g;
    GD_gvc(g) = gvc;
    gvLayout(gvc, g, layout);
    job->selected_obj = NULL;
    job->current_obj = NULL;
    job->needs_refresh = 1;
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
