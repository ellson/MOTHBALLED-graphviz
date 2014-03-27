/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#include <time.h>
#include "dot.h"
#include "pack.h"
#include "aspect.h"

static void
dot_init_subg(graph_t * g, graph_t* droot)
{
    graph_t* subg;

    if ((g != agroot(g)))
	agbindrec(g, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);
    if (g == droot)
	GD_dotroot(agroot(g)) = droot;
	
    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	dot_init_subg(subg, droot);
    }
}


static void 
dot_init_node(node_t * n)
{
    agbindrec(n, "Agnodeinfo_t", sizeof(Agnodeinfo_t), TRUE);	//graph custom data
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
    agbindrec(e, "Agedgeinfo_t", sizeof(Agedgeinfo_t), TRUE);	//graph custom data
    common_init_edge(e);

    ED_weight(e) = late_int(e, E_weight, 1, 0);
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
    free_label(ND_xlabel(n));
    if (ND_shape(n))
	ND_shape(n)->fns->freefn(n);
    agdelrec(n, "Agnodeinfo_t");	
}

static void free_virtual_edge_list(node_t * n)
{
    edge_t *e;
    int i;

    for (i = ND_in(n).size - 1; i >= 0; i--) {
	e = ND_in(n).list[i];
	delete_fast_edge(e);
	free(e->base.data);
	free(e);
    }
    for (i = ND_out(n).size - 1; i >= 0; i--) {
	e = ND_out(n).list[i];
	delete_fast_edge(e);
	free(e->base.data);
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
	    free(vn->base.data);
	    free(vn);
	}
	vn = next_vn;
    }
}

static void 
dot_cleanup_graph(graph_t * g)
{
    int i;
    graph_t *subg;
    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	dot_cleanup_graph(subg);
    }
    if (GD_clust(g)) free (GD_clust(g));
    if (GD_rankleader(g)) free (GD_rankleader(g));

    free_list(GD_comp(g));
    if (GD_rank(g)) {
	for (i = GD_minrank(g); i <= GD_maxrank(g); i++)
	    free(GD_rank(g)[i].av);
	if (GD_minrank(g) == -1)
	    free(GD_rank(g)-1);
	else
	    free(GD_rank(g));
    }
    if (g != agroot(g)) 
	agdelrec(g,"Agraphinfo_t");
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
	    if (streq(agnameof(u),"virtual"))
	        fprintf (stderr, " %x", u);
	    else
	        fprintf (stderr, " %s", agnameof(u));
      
        }
	fprintf (stderr, "\n");
    }
    fprintf (stderr, "count %d rank count = %d\n", fastn(g), rcnt);
}
#endif


static void
remove_from_rank (Agraph_t * g, Agnode_t* n)
{
    Agnode_t* v = NULL;
    int j, rk = ND_rank(n);

    for (j = 0; j < GD_rank(g)[rk].n; j++) {
	v = GD_rank(g)[rk].v[j];
	if (v == n) {
	    for (j++; j < GD_rank(g)[rk].n; j++) {
		GD_rank(g)[rk].v[j-1] = GD_rank(g)[rk].v[j];
	    }
	    GD_rank(g)[rk].n--;
	    break;
	}
    }
    assert (v == n);  /* if found */
}

/* removeFill:
 * This removes all of the fill nodes added in mincross.
 * It appears to be sufficient to remove them only from the
 * rank array and fast node list of the root graph.
 */
static void
removeFill (Agraph_t * g)
{
    Agnode_t* n;
    Agnode_t* nxt;
    Agraph_t* sg = agsubg (g, "_new_rank", 0);

    if (!sg) return;
    for (n = agfstnode(sg); n; n = nxt) {
	nxt = agnxtnode(sg, n);
	delete_fast_node (g, n);
	remove_from_rank (g, n);
	dot_cleanup_node (n);
	agdelnode(g, n);
    }
    agdelsubg (g, sg);

}

#define ag_xset(x,a,v) agxset(x,a,v)
#define agnodeattr(g,n,v) agattr(g,AGNODE,n,v)

static void
attach_phase_attrs (Agraph_t * g, int maxphase)
{
    Agsym_t* rk = agnodeattr(g,"rank","");
    Agsym_t* order = agnodeattr(g,"order","");
    Agnode_t* n;
    char buf[BUFSIZ];

    for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
	if (maxphase >= 1) {
	    sprintf(buf, "%d", ND_rank(n));
	    ag_xset(n,rk,buf);
	}
	if (maxphase >= 2) {
	    sprintf(buf, "%d", ND_order(n));
	    ag_xset(n,order,buf);
	}
    }
}

static void dotLayout(Agraph_t * g)
{
    aspect_t aspect;
    aspect_t* asp;
    int maxphase = late_int(g, agfindgraphattr(g,"phase"), -1, 1);

    setEdgeType (g, ET_SPLINE);
    asp = setAspect (g, &aspect);

    dot_init_subg(g,g);
    dot_init_node_edge(g);

    do {
        dot_rank(g, asp);
	if (maxphase == 1) {
	    attach_phase_attrs (g, 1);
	    return;
	}
	if (aspect.badGraph) {
	    agerr(AGWARN, "dot does not support the aspect attribute for disconnected graphs or graphs with clusters\n");
	    asp = NULL;
	    aspect.nextIter = 0;
	}
        dot_mincross(g, (asp != NULL));
	if (maxphase == 2) {
	    attach_phase_attrs (g, 2);
	    return;
	}
        dot_position(g, asp);
	if (maxphase == 3) {
	    attach_phase_attrs (g, 2);  /* positions will be attached on output */
	    return;
	}
	aspect.nPasses--;
    } while (aspect.nextIter && aspect.nPasses);
    if (GD_flags(g) & NEW_RANK)
	removeFill (g);
    dot_sameports(g);
    dot_splines(g);
    if (mapbool(agget(g, "compound")))
	dot_compoundEdges(g);
}

static void
initSubg (Agraph_t* sg, Agraph_t* g)
{
    agbindrec(sg, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);
    GD_drawing(sg) = NEW(layout_t);
    GD_drawing(sg)->quantum = GD_drawing(g)->quantum; 
    GD_drawing(sg)->dpi = GD_drawing(g)->dpi;
    GD_gvc(sg) = GD_gvc (g);
    GD_charset(sg) = GD_charset (g);
    GD_rankdir2(sg) = GD_rankdir2 (g);
    GD_nodesep(sg) = GD_nodesep(g);
    GD_ranksep(sg) = GD_ranksep(g);
    GD_fontnames(sg) = GD_fontnames(g);
}

static void
attachPos (Agraph_t* g)
{
    node_t* np;
    double* ps = N_NEW(2*agnnodes(g), double);

    for (np = agfstnode(g); np; np = agnxtnode(g, np)) {
	ND_pos(np) = ps;
	ps[0] = PS2INCH(ND_coord(np).x);
	ps[1] = PS2INCH(ND_coord(np).y);
	ps += 2;
    }
}

static void
resetCoord (Agraph_t* g)
{
    node_t* np = agfstnode(g);
    double* sp = ND_pos(np);
    double* ps = sp;

    for (np = agfstnode(g); np; np = agnxtnode(g, np)) {
	ND_pos(np) = 0;
	ND_coord(np).x = INCH2PS(ps[0]);
	ND_coord(np).y = INCH2PS(ps[1]);
	ps += 2;
    }
    free (sp);
}

/* doDot:
 * Assume g has nodes.
 */
static void doDot (Agraph_t* g)
{
    Agraph_t **ccs;
    Agraph_t *sg;
    int ncc;
    int i;
    pack_info pinfo;
    int Pack = getPack(g, -1, CL_OFFSET);
    pack_mode mode = getPackModeInfo (g, l_undef, &pinfo);
    getPackInfo(g, l_node, CL_OFFSET, &pinfo);

    if ((mode == l_undef) && (Pack < 0)) {
	/* No pack information; use old dot with components
         * handled during layout
         */
	dotLayout(g);
    } else {
	/* fill in default values */
	if (mode == l_undef) 
	    pinfo.mode = l_node;
	else if (Pack < 0)
	    Pack = CL_OFFSET;
	pinfo.margin = Pack;
	pinfo.fixed = 0;

          /* components using clusters */
	ccs = cccomps(g, &ncc, 0);
	if (ncc == 1) {
	    dotLayout(g);
	} else if (GD_drawing(g)->ratio_kind == R_NONE) {
	    pinfo.doSplines = 1;

	    for (i = 0; i < ncc; i++) {
		sg = ccs[i];
		initSubg (sg, g);
		dotLayout (sg);
	    }
	    attachPos (g);
	    packSubgraphs(ncc, ccs, g, &pinfo);
	    resetCoord (g);
	} else {
	    /* Not sure what semantics should be for non-trivial ratio
             * attribute with multiple components.
             * One possibility is to layout nodes, pack, then apply the ratio
             * adjustment. We would then have to re-adjust all positions.
             */
	    dotLayout(g);
	}

	for (i = 0; i < ncc; i++) {
	    free (GD_drawing(ccs[i]));
	    agdelete(g, ccs[i]);
	}
	free(ccs);
    }
}

void dot_layout(Agraph_t * g)
{
    if (agnnodes(g)) doDot (g);
    dotneato_postprocess(g);
}

Agraph_t * dot_root (void* p)
{
    return GD_dotroot(agroot(p));
}

