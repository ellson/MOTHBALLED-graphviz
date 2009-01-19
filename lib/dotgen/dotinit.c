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


#include <time.h>
#include "dot.h"
#include "aspect.h"

static void 
dot_init_node(node_t * n)
{
#ifdef WITH_CGRAPH
    agbindrec(n, "Agnodeinfo_t", sizeof(Agnodeinfo_t), TRUE);	//graph custom data
#endif /* WITH_CGRAPH */
    common_init_node(n);
    gv_nodesize(n, GD_flip(agraphof(n)));
    alloc_elist(4, ND_in(n));
    alloc_elist(4, ND_out(n));
    alloc_elist(2, ND_flat_in(n));
    alloc_elist(2, ND_flat_out(n));
    alloc_elist(2, ND_other(n));
    ND_UF_size(n) = 1;
}

static void 
dot_init_edge(edge_t * e)
{
    char *tailgroup, *headgroup;
#ifdef WITH_CGRAPH
    agbindrec(e, "Agedgeinfo_t", sizeof(Agedgeinfo_t), TRUE);	//graph custom data
#endif /* WITH_CGRAPH */
    common_init_edge(e);

    ED_weight(e) = late_double(e, E_weight, 1.0, 0.0);
    tailgroup = late_string(agtail(e), N_group, "");
    headgroup = late_string(aghead(e), N_group, "");
    ED_count(e) = ED_xpenalty(e) = 1;
    if (tailgroup[0] && (tailgroup == headgroup)) {
	ED_xpenalty(e) = CL_CROSS;
	ED_weight(e) *= 100;
    }
    if (nonconstraint_edge(e)) {
	ED_xpenalty(e) = 0;
	ED_weight(e) = 0;
    }

    ED_showboxes(e) = late_int(e, E_showboxes, 0, 0);
    ED_minlen(e) = late_int(e, E_minlen, 1, 0);
}

void 
dot_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	dot_init_node(n);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    dot_init_edge(e);
    }
}

#if 0				/* not used */
static void free_edge_list(elist L)
{
    edge_t *e;
    int i;

    for (i = 0; i < L.size; i++) {
	e = L.list[i];
	free(e);
    }
}
#endif

static void 
dot_cleanup_node(node_t * n)
{
    free_list(ND_in(n));
    free_list(ND_out(n));
    free_list(ND_flat_out(n));
    free_list(ND_flat_in(n));
    free_list(ND_other(n));
    free_label(ND_label(n));
    if (ND_shape(n))
	ND_shape(n)->fns->freefn(n);
#ifndef WITH_CGRAPH
    memset(&(n->u), 0, sizeof(Agnodeinfo_t));
#else /* WITH_CGRAPH */
	agclean(agraphof(n), AGNODE,"Agnodeinfo_t");	
#endif /* WITH_CGRAPH */
}

static void free_virtual_edge_list(node_t * n)
{
    edge_t *e;
    int i;

    for (i = ND_in(n).size - 1; i >= 0; i--) {
	e = ND_in(n).list[i];
	delete_fast_edge(e);
	free(e);
    }
    for (i = ND_out(n).size - 1; i >= 0; i--) {
	e = ND_out(n).list[i];
	delete_fast_edge(e);
	free(e);
    }
}

static void free_virtual_node_list(node_t * vn)
{
    node_t *next_vn;

    while (vn) {
	next_vn = ND_next(vn);
	free_virtual_edge_list(vn);
	if (ND_node_type(vn) == VIRTUAL) {
	    free_list(ND_out(vn));
	    free_list(ND_in(vn));
	    free(vn);
	}
	vn = next_vn;
    }
}

static void 
dot_cleanup_graph(graph_t * g)
{
    int i, c;
    graph_t *clust;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	clust = GD_clust(g)[c];
	GD_cluster_was_collapsed(clust) = FALSE;
	dot_cleanup(clust);
    }

    free_list(GD_comp(g));
    if ((g == agroot(g)) && GD_rank(g)) {
	for (i = GD_minrank(g); i <= GD_maxrank(g); i++)
	    free(GD_rank(g)[i].v);
	free(GD_rank(g));
    }
    if (g != agroot(g)) 
#ifndef WITH_CGRAPH
	memset(&(g->u), 0, sizeof(Agraphinfo_t));
#else /* WITH_CGRAPH */
	agclean(g,AGRAPH,"Agraphinfo_t");
#endif /* WITH_CGRAPH */
}

/* delete the layout (but retain the underlying graph) */
void dot_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    free_virtual_node_list(GD_nlist(g));
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    gv_cleanup_edge(e);
	}
	dot_cleanup_node(n);
    }
    dot_cleanup_graph(g);
}

#ifdef DEBUG
int
fastn (graph_t * g)
{
    node_t* u;
    int cnt = 0;
    for (u = GD_nlist(g); u; u = ND_next(u)) cnt++;
    return cnt;
}

static void
dumpRanks (graph_t * g)
{
    int i, j;
    node_t* u;
    rank_t *rank = GD_rank(g);
    int rcnt = 0;
    for (i = GD_minrank(g); i <= GD_maxrank(g); i++) {
	fprintf (stderr, "[%d] :", i);
	for (j = 0; j < rank[i].n; j++) {
	    u = rank[i].v[j];
            rcnt++;
	    if (streq(u->name,"virtual"))
	        fprintf (stderr, " %x", u);
	    else
	        fprintf (stderr, " %s", u->name);
      
        }
	fprintf (stderr, "\n");
    }
    fprintf (stderr, "count %d rank count = %d\n", fastn(g), rcnt);
}
#endif

#define MIN_AR 1.0
#define MAX_AR 20.0
#define DEF_PASSES 5

static aspect_t*
setAspect (Agraph_t * g, aspect_t* adata)
{
    double rv;
    char *p;
    int r, passes = DEF_PASSES;

    p = agget (g, "aspect");

    if (!p || ((r = sscanf (p, "%lf,%d", &rv, &passes)) <= 0)) {
	adata->nextIter = 0;
	adata->badGraph = 0;
	return NULL;
    }
    
    if (rv < MIN_AR) rv = MIN_AR;
    else if (rv > MAX_AR) rv = MAX_AR;
    adata->targetAR = rv;
    adata->nextIter = -1;
    adata->nPasses = passes;
    adata->badGraph = 0;

    if (Verbose) 
        fprintf(stderr, "Target AR = %g\n", adata->targetAR);

    return adata;
}

void dot_layout(Agraph_t * g)
{
    aspect_t aspect;
    aspect_t* asp;

    setEdgeType (g, ET_SPLINE);
    asp = setAspect (g, &aspect);

    dot_init_node_edge(g);

    do {
        dot_rank(g, asp);
	if (aspect.badGraph) {
	    agerr(AGWARN, "dot does not support the aspect attribute for disconnected graphs or graphs with clusters\n");
	    asp = NULL;
	    aspect.nextIter = 0;
	}
        dot_mincross(g,  (asp != NULL));
        dot_position(g, asp);
	aspect.nPasses--;
    } while (aspect.nextIter && aspect.nPasses);

    dot_sameports(g);
    dot_splines(g);
    if (mapbool(agget(g, "compound")))
	dot_compoundEdges(g);
    dotneato_postprocess(g);

}
