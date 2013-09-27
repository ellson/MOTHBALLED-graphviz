#include "newdot.h"

static void reverse_edge(graph_t *g, edge_t *e)
{
	edge_t	*rev;

	rev = agfindedge(g,e->head,e->tail);
	if (!rev) rev = agedge(g,e->head,e->tail);
	merge(rev,ED_minlen(e),ED_weight(e));
	agdelete(g,e);
}

static void dfs(graph_t *g, node_t *v)
{
    edge_t  *e, *f;
    node_t  *w;
                                                                                
    if (ND_mark(v)) return;
    ND_mark(v) = TRUE;
    ND_onstack(v) = TRUE;
	for (e = agfstout(g,v); e; e = f) {
		f = agnxtout(g,e);
        w = e->head;
        if (ND_onstack(w)) reverse_edge(g,e);
        else { if (ND_mark(w) == FALSE) dfs(g,w); }
    }
    ND_onstack(v) = FALSE;
}

static void break_cycles(graph_t *g)
{
	node_t	*n;

	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		ND_mark(n) = ND_onstack(n) = FALSE;
	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		dfs(g,n);
}
