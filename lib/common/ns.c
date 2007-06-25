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


/* 
 * Network Simplex Algorithm for Ranking Nodes of a DAG
 */

#include "render.h"

static int init_graph(graph_t *);
static void dfs_cutval(node_t * v, edge_t * par);
static int dfs_range(node_t * v, edge_t * par, int low);
static int x_val(edge_t * e, node_t * v, int dir);
#ifdef DEBUG
static void check_cycles(graph_t * g);
#endif

#define LENGTH(e)		(ND_rank(e->head) - ND_rank(e->tail))
#define SLACK(e)		(LENGTH(e) - ED_minlen(e))
#define SEQ(a,b,c)		(((a) <= (b)) && ((b) <= (c)))
#define TREE_EDGE(e)	(ED_tree_index(e) >= 0)

static graph_t *G;
static int N_nodes, N_edges;
static int Minrank, Maxrank;
static int S_i;			/* search index for enter_edge */
static int Search_size;
#define SEARCHSIZE 30
static nlist_t Tree_node;
static elist Tree_edge;

static void add_tree_edge(edge_t * e)
{
    node_t *n;
    if (TREE_EDGE(e))
	abort();
    ED_tree_index(e) = Tree_edge.size;
    Tree_edge.list[Tree_edge.size++] = e;
    if (ND_mark(e->tail) == FALSE)
	Tree_node.list[Tree_node.size++] = e->tail;
    if (ND_mark(e->head) == FALSE)
	Tree_node.list[Tree_node.size++] = e->head;
    n = e->tail;
    ND_mark(n) = TRUE;
    ND_tree_out(n).list[ND_tree_out(n).size++] = e;
    ND_tree_out(n).list[ND_tree_out(n).size] = NULL;
    if (ND_out(n).list[ND_tree_out(n).size - 1] == 0)
	abort();
    n = e->head;
    ND_mark(n) = TRUE;
    ND_tree_in(n).list[ND_tree_in(n).size++] = e;
    ND_tree_in(n).list[ND_tree_in(n).size] = NULL;
    if (ND_in(n).list[ND_tree_in(n).size - 1] == 0)
	abort();
}

static void exchange_tree_edges(edge_t * e, edge_t * f)
{
    int i, j;
    node_t *n;

    ED_tree_index(f) = ED_tree_index(e);
    Tree_edge.list[ED_tree_index(e)] = f;
    ED_tree_index(e) = -1;

    n = e->tail;
    i = --(ND_tree_out(n).size);
    for (j = 0; j <= i; j++)
	if (ND_tree_out(n).list[j] == e)
	    break;
    ND_tree_out(n).list[j] = ND_tree_out(n).list[i];
    ND_tree_out(n).list[i] = NULL;
    n = e->head;
    i = --(ND_tree_in(n).size);
    for (j = 0; j <= i; j++)
	if (ND_tree_in(n).list[j] == e)
	    break;
    ND_tree_in(n).list[j] = ND_tree_in(n).list[i];
    ND_tree_in(n).list[i] = NULL;

    n = f->tail;
    ND_tree_out(n).list[ND_tree_out(n).size++] = f;
    ND_tree_out(n).list[ND_tree_out(n).size] = NULL;
    n = f->head;
    ND_tree_in(n).list[ND_tree_in(n).size++] = f;
    ND_tree_in(n).list[ND_tree_in(n).size] = NULL;
}

static
void init_rank(void)
{
    int i, ctr;
    nodequeue *Q;
    node_t *v;
    edge_t *e;

    Q = new_queue(N_nodes);
    ctr = 0;

    for (v = GD_nlist(G); v; v = ND_next(v)) {
	if (ND_priority(v) == 0)
	    enqueue(Q, v);
    }

    while ((v = dequeue(Q))) {
	ND_rank(v) = 0;
	ctr++;
	for (i = 0; (e = ND_in(v).list[i]); i++)
	    ND_rank(v) = MAX(ND_rank(v), ND_rank(e->tail) + ED_minlen(e));
	for (i = 0; (e = ND_out(v).list[i]); i++) {
	    if (--(ND_priority(e->head)) <= 0)
		enqueue(Q, e->head);
	}
    }
    if (ctr != N_nodes) {
	agerr(AGERR, "trouble in init_rank\n");
	for (v = GD_nlist(G); v; v = ND_next(v))
	    if (ND_priority(v))
		agerr(AGPREV, "\t%s %d\n", v->name, ND_priority(v));
    }
    free_queue(Q);
}

static node_t *incident(edge_t * e)
{
    if (ND_mark(e->tail)) {
	if (ND_mark(e->head) == FALSE)
	    return e->tail;
    } else {
	if (ND_mark(e->head))
	    return e->head;
    }
    return NULL;
}

static edge_t *leave_edge(void)
{
    edge_t *f, *rv = NULL;
    int j, cnt = 0;

    j = S_i;
    while (S_i < Tree_edge.size) {
	if ((f = Tree_edge.list[S_i])->u.cutvalue < 0) {
	    if (rv) {
		if (ED_cutvalue(rv) > ED_cutvalue(f))
		    rv = f;
	    } else
		rv = Tree_edge.list[S_i];
	    if (++cnt >= Search_size)
		return rv;
	}
	S_i++;
    }
    if (j > 0) {
	S_i = 0;
	while (S_i < j) {
	    if ((f = Tree_edge.list[S_i])->u.cutvalue < 0) {
		if (rv) {
		    if (ED_cutvalue(rv) > ED_cutvalue(f))
			rv = f;
		} else
		    rv = Tree_edge.list[S_i];
		if (++cnt >= Search_size)
		    return rv;
	    }
	    S_i++;
	}
    }
    return rv;
}

static edge_t *Enter;
static int Low, Lim, Slack;

static void dfs_enter_outedge(node_t * v)
{
    int i, slack;
    edge_t *e;

    for (i = 0; (e = ND_out(v).list[i]); i++) {
	if (TREE_EDGE(e) == FALSE) {
	    if (!SEQ(Low, ND_lim(e->head), Lim)) {
		slack = SLACK(e);
		if ((slack < Slack) || (Enter == NULL)) {
		    Enter = e;
		    Slack = slack;
		}
	    }
	} else if (ND_lim(e->head) < ND_lim(v))
	    dfs_enter_outedge(e->head);
    }
    for (i = 0; (e = ND_tree_in(v).list[i]) && (Slack > 0); i++)
	if (ND_lim(e->tail) < ND_lim(v))
	    dfs_enter_outedge(e->tail);
}

static void dfs_enter_inedge(node_t * v)
{
    int i, slack;
    edge_t *e;

    for (i = 0; (e = ND_in(v).list[i]); i++) {
	if (TREE_EDGE(e) == FALSE) {
	    if (!SEQ(Low, ND_lim(e->tail), Lim)) {
		slack = SLACK(e);
		if ((slack < Slack) || (Enter == NULL)) {
		    Enter = e;
		    Slack = slack;
		}
	    }
	} else if (ND_lim(e->tail) < ND_lim(v))
	    dfs_enter_inedge(e->tail);
    }
    for (i = 0; (e = ND_tree_out(v).list[i]) && (Slack > 0); i++)
	if (ND_lim(e->head) < ND_lim(v))
	    dfs_enter_inedge(e->head);
}

static edge_t *enter_edge(edge_t * e)
{
    node_t *v;
    int outsearch;

    /* v is the down node */
    if (ND_lim(e->tail) < ND_lim(e->head)) {
	v = e->tail;
	outsearch = FALSE;
    } else {
	v = e->head;
	outsearch = TRUE;
    }
    Enter = NULL;
    Slack = INT_MAX;
    Low = ND_low(v);
    Lim = ND_lim(v);
    if (outsearch)
	dfs_enter_outedge(v);
    else
	dfs_enter_inedge(v);
    return Enter;
}

static int treesearch(node_t * v)
{
    int i;
    edge_t *e;

    for (i = 0; (e = ND_out(v).list[i]); i++) {
	if ((ND_mark(e->head) == FALSE) && (SLACK(e) == 0)) {
	    add_tree_edge(e);
	    if ((Tree_edge.size == N_nodes - 1) || treesearch(e->head))
		return TRUE;
	}
    }
    for (i = 0; (e = ND_in(v).list[i]); i++) {
	if ((ND_mark(e->tail) == FALSE) && (SLACK(e) == 0)) {
	    add_tree_edge(e);
	    if ((Tree_edge.size == N_nodes - 1) || treesearch(e->tail))
		return TRUE;
	}
    }
    return FALSE;
}

static int tight_tree(void)
{
    int i;
    node_t *n;

    for (n = GD_nlist(G); n; n = ND_next(n)) {
	ND_mark(n) = FALSE;
	ND_tree_in(n).list[0] = ND_tree_out(n).list[0] = NULL;
	ND_tree_in(n).size = ND_tree_out(n).size = 0;
    }
    for (i = 0; i < Tree_edge.size; i++)
	Tree_edge.list[i]->u.tree_index = -1;

    Tree_node.size = Tree_edge.size = 0;
    for (n = GD_nlist(G); n && (Tree_edge.size == 0); n = ND_next(n))
	treesearch(n);
    return Tree_node.size;
}

static void init_cutvalues(void)
{
    dfs_range(GD_nlist(G), NULL, 1);
    dfs_cutval(GD_nlist(G), NULL);
}

static int feasible_tree(void)
{
    int i, delta;
    node_t *n;
    edge_t *e, *f;

    if (N_nodes <= 1)
	return 0;
    while (tight_tree() < N_nodes) {
	e = NULL;
	for (n = GD_nlist(G); n; n = ND_next(n)) {
	    for (i = 0; (f = ND_out(n).list[i]); i++) {
		if ((TREE_EDGE(f) == FALSE) && incident(f) && ((e == NULL)
							       || (SLACK(f)
								   <
								   SLACK
								   (e))))
		    e = f;
	    }
	}
	if (e) {
	    delta = SLACK(e);
	    if (delta) {
		if (incident(e) == e->head)
		    delta = -delta;
		for (i = 0; i < Tree_node.size; i++)
		    Tree_node.list[i]->u.rank += delta;
	    }
	} else {
#ifdef DEBUG
	    fprintf(stderr, "not in tight tree:\n");
	    for (n = GD_nlist(G); n; n = ND_next(n)) {
		for (i = 0; i < Tree_node.size; i++)
		    if (Tree_node.list[i] == n)
			break;
		if (i >= Tree_node.size)
		    fprintf(stderr, "\t%s\n", n->name);
	    }
#endif
	    return 1;
	}
    }
    init_cutvalues();
    return 0;
}

/* walk up from v to LCA(v,w), setting new cutvalues. */
static node_t *treeupdate(node_t * v, node_t * w, int cutvalue, int dir)
{
    edge_t *e;
    int d;

    while (!SEQ(ND_low(v), ND_lim(w), ND_lim(v))) {
	e = ND_par(v);
	if (v == e->tail)
	    d = dir;
	else
	    d = NOT(dir);
	if (d)
	    ED_cutvalue(e) += cutvalue;
	else
	    ED_cutvalue(e) -= cutvalue;
	if (ND_lim(e->tail) > ND_lim(e->head))
	    v = e->tail;
	else
	    v = e->head;
    }
    return v;
}

static void rerank(node_t * v, int delta)
{
    int i;
    edge_t *e;

    ND_rank(v) -= delta;
    for (i = 0; (e = ND_tree_out(v).list[i]); i++)
	if (e != ND_par(v))
	    rerank(e->head, delta);
    for (i = 0; (e = ND_tree_in(v).list[i]); i++)
	if (e != ND_par(v))
	    rerank(e->tail, delta);
}

/* e is the tree edge that is leaving and f is the nontree edge that
 * is entering.  compute new cut values, ranks, and exchange e and f.
 */
static void 
update(edge_t * e, edge_t * f)
{
    int cutvalue, delta;
    node_t *lca;

    delta = SLACK(f);
    /* "for (v = in nodes in tail side of e) do ND_rank(v) -= delta;" */
    if (delta > 0) {
	int s;
	s = ND_tree_in(e->tail).size + ND_tree_out(e->tail).size;
	if (s == 1)
	    rerank(e->tail, delta);
	else {
	    s = ND_tree_in(e->head).size + ND_tree_out(e->head).size;
	    if (s == 1)
		rerank(e->head, -delta);
	    else {
		if (ND_lim(e->tail) < ND_lim(e->head))
		    rerank(e->tail, delta);
		else
		    rerank(e->head, -delta);
	    }
	}
    }

    cutvalue = ED_cutvalue(e);
    lca = treeupdate(f->tail, f->head, cutvalue, 1);
    if (treeupdate(f->head, f->tail, cutvalue, 0) != lca)
	abort();
    ED_cutvalue(f) = -cutvalue;
    ED_cutvalue(e) = 0;
    exchange_tree_edges(e, f);
    dfs_range(lca, ND_par(lca), ND_low(lca));
}

static void scan_and_normalize(void)
{
    node_t *n;

    Minrank = INT_MAX;
    Maxrank = -INT_MAX;
    for (n = GD_nlist(G); n; n = ND_next(n)) {
	if (ND_node_type(n) == NORMAL) {
	    Minrank = MIN(Minrank, ND_rank(n));
	    Maxrank = MAX(Maxrank, ND_rank(n));
	}
    }
    if (Minrank != 0) {
	for (n = GD_nlist(G); n; n = ND_next(n))
	    ND_rank(n) -= Minrank;
	Maxrank -= Minrank;
	Minrank = 0;
    }
}

static void
freeTreeList (graph_t* g)
{
    node_t *n;
    for (n = GD_nlist(G); n; n = ND_next(n)) {
	free_list(ND_tree_in(n));
	free_list(ND_tree_out(n));
	ND_mark(n) = FALSE;
    }
}

static void LR_balance(void)
{
    int i, delta;
    edge_t *e, *f;

    for (i = 0; i < Tree_edge.size; i++) {
	e = Tree_edge.list[i];
	if (ED_cutvalue(e) == 0) {
	    f = enter_edge(e);
	    if (f == NULL)
		continue;
	    delta = SLACK(f);
	    if (delta <= 1)
		continue;
	    if (ND_lim(e->tail) < ND_lim(e->head))
		rerank(e->tail, delta / 2);
	    else
		rerank(e->head, -delta / 2);
	}
    }
    freeTreeList (G);
}

static void TB_balance(void)
{
    node_t *n;
    edge_t *e;
    int i, low, high, choice, *nrank;
    int inweight, outweight;

    scan_and_normalize();

    /* find nodes that are not tight and move to less populated ranks */
    nrank = N_NEW(Maxrank + 1, int);
    for (i = 0; i <= Maxrank; i++)
	nrank[i] = 0;
    for (n = GD_nlist(G); n; n = ND_next(n))
	if (ND_node_type(n) == NORMAL)
	    nrank[ND_rank(n)]++;
    for (n = GD_nlist(G); n; n = ND_next(n)) {
	if (ND_node_type(n) != NORMAL)
	    continue;
	inweight = outweight = 0;
	low = 0;
	high = Maxrank;
	for (i = 0; (e = ND_in(n).list[i]); i++) {
	    inweight += ED_weight(e);
	    low = MAX(low, ND_rank(e->tail) + ED_minlen(e));
	}
	for (i = 0; (e = ND_out(n).list[i]); i++) {
	    outweight += ED_weight(e);
	    high = MIN(high, ND_rank(e->head) - ED_minlen(e));
	}
	if (low < 0)
	    low = 0;		/* vnodes can have ranks < 0 */
	if (inweight == outweight) {
	    choice = low;
	    for (i = low + 1; i <= high; i++)
		if (nrank[i] < nrank[choice])
		    choice = i;
	    nrank[ND_rank(n)]--;
	    nrank[choice]++;
	    ND_rank(n) = choice;
	}
	free_list(ND_tree_in(n));
	free_list(ND_tree_out(n));
	ND_mark(n) = FALSE;
    }
    free(nrank);
}

static int init_graph(graph_t * g)
{
    int i, feasible;
    node_t *n;
    edge_t *e;

    G = g;
    N_nodes = N_edges = S_i = 0;
    for (n = GD_nlist(g); n; n = ND_next(n)) {
	ND_mark(n) = FALSE;
	N_nodes++;
	for (i = 0; (e = ND_out(n).list[i]); i++)
	    N_edges++;
    }

    Tree_node.list = ALLOC(N_nodes, Tree_node.list, node_t *);
    Tree_node.size = 0;
    Tree_edge.list = ALLOC(N_nodes, Tree_edge.list, edge_t *);
    Tree_edge.size = 0;

    feasible = TRUE;
    for (n = GD_nlist(g); n; n = ND_next(n)) {
	ND_priority(n) = 0;
	for (i = 0; (e = ND_in(n).list[i]); i++) {
	    ND_priority(n)++;
	    ED_cutvalue(e) = 0;
	    ED_tree_index(e) = -1;
	    if (feasible
		&& (ND_rank(e->head) - ND_rank(e->tail) < ED_minlen(e)))
		feasible = FALSE;
	}
	ND_tree_in(n).list = N_NEW(i + 1, edge_t *);
	ND_tree_in(n).size = 0;
	for (i = 0; (e = ND_out(n).list[i]); i++);
	ND_tree_out(n).list = N_NEW(i + 1, edge_t *);
	ND_tree_out(n).size = 0;
    }
    return feasible;
}

/* rank:
 * Apply network simplex to rank the nodes in a graph.
 * Uses ED_minlen as the internode constraint: if a->b with minlen=ml,
 * rank b - rank a >= ml.
 * Assumes the graph has the following additional structure:
 *   A list of all nodes, starting at GD_nlist, and linked using ND_next.
 *   Out and in edges lists stored in ND_out and ND_in, even if the node
 *  doesn't have any out or in edges.
 * The node rank values are stored in ND_rank.
 * Returns 0 if successful; returns 1 if not, the latter indicating that
 * the graph was not connected.
 */
int rank(graph_t * g, int balance, int maxiter)
{
    int iter = 0, feasible;
    char *s, *ns = "network simplex: ";
    edge_t *e, *f;

#ifdef DEBUG
    check_cycles(g);
#endif
    if (Verbose)
	start_timer();
    feasible = init_graph(g);
    if (!feasible)
	init_rank();
    if (maxiter <= 0) {
	freeTreeList (g);
	return 0;
    }

    if ((s = agget(g, "searchsize")))
	Search_size = atoi(s);
    else
	Search_size = SEARCHSIZE;

    if (feasible_tree()) {
	freeTreeList (g);
	return 1;
    }
    while ((e = leave_edge())) {
	f = enter_edge(e);
	update(e, f);
	iter++;
	if (Verbose && (iter % 100 == 0)) {
	    if (iter % 1000 == 100)
		fputs(ns, stderr);
	    fprintf(stderr, "%d ", iter);
	    if (iter % 1000 == 0)
		fputc('\n', stderr);
	}
	if (iter >= maxiter)
	    break;
    }
    switch (balance) {
    case 1:
	TB_balance();
	break;
    case 2:
	LR_balance();
	break;
    default:
	scan_and_normalize();
	break;
    }
    if (Verbose) {
	if (iter >= 100)
	    fputc('\n', stderr);
	fprintf(stderr, "%s%d nodes %d edges %d iter %.2f sec\n",
		ns, N_nodes, N_edges, iter, elapsed_sec());
    }
    return 0;
}

/* set cut value of f, assuming values of edges on one side were already set */
static void x_cutval(edge_t * f)
{
    node_t *v;
    edge_t *e;
    int i, sum, dir;

    /* set v to the node on the side of the edge already searched */
    if (ND_par(f->tail) == f) {
	v = f->tail;
	dir = 1;
    } else {
	v = f->head;
	dir = -1;
    }

    sum = 0;
    for (i = 0; (e = ND_out(v).list[i]); i++)
	sum += x_val(e, v, dir);
    for (i = 0; (e = ND_in(v).list[i]); i++)
	sum += x_val(e, v, dir);
    ED_cutvalue(f) = sum;
}

static int x_val(edge_t * e, node_t * v, int dir)
{
    node_t *other;
    int d, rv, f;

    if (e->tail == v)
	other = e->head;
    else
	other = e->tail;
    if (!(SEQ(ND_low(v), ND_lim(other), ND_lim(v)))) {
	f = 1;
	rv = ED_weight(e);
    } else {
	f = 0;
	if (TREE_EDGE(e))
	    rv = ED_cutvalue(e);
	else
	    rv = 0;
	rv -= ED_weight(e);
    }
    if (dir > 0) {
	if (e->head == v)
	    d = 1;
	else
	    d = -1;
    } else {
	if (e->tail == v)
	    d = 1;
	else
	    d = -1;
    }
    if (f)
	d = -d;
    if (d < 0)
	rv = -rv;
    return rv;
}

static void dfs_cutval(node_t * v, edge_t * par)
{
    int i;
    edge_t *e;

    for (i = 0; (e = ND_tree_out(v).list[i]); i++)
	if (e != par)
	    dfs_cutval(e->head, e);
    for (i = 0; (e = ND_tree_in(v).list[i]); i++)
	if (e != par)
	    dfs_cutval(e->tail, e);
    if (par)
	x_cutval(par);
}

static int dfs_range(node_t * v, edge_t * par, int low)
{
    edge_t *e;
    int i, lim;

    lim = low;
    ND_par(v) = par;
    ND_low(v) = low;
    for (i = 0; (e = ND_tree_out(v).list[i]); i++)
	if (e != par)
	    lim = dfs_range(e->head, e, lim);
    for (i = 0; (e = ND_tree_in(v).list[i]); i++)
	if (e != par)
	    lim = dfs_range(e->tail, e, lim);
    ND_lim(v) = lim;
    return lim + 1;
}

#ifdef DEBUG
void tchk(void)
{
    int i, n_cnt, e_cnt;
    node_t *n;
    edge_t *e;

    n_cnt = 0;
    e_cnt = 0;
    for (n = GD_nlist(G); n; n = ND_next(n)) {
	n_cnt++;
	for (i = 0; (e = ND_tree_out(n).list[i]); i++) {
	    e_cnt++;
	    if (SLACK(e) > 0)
		printf("not a tight tree %x", e);
	}
    }
    if ((n_cnt != Tree_node.size) || (e_cnt != Tree_edge.size))
	printf("something missing\n");
}

void check_cutvalues(void)
{
    node_t *v;
    edge_t *e;
    int i, save;

    for (v = GD_nlist(G); v; v = ND_next(v)) {
	for (i = 0; (e = ND_tree_out(v).list[i]); i++) {
	    save = ED_cutvalue(e);
	    x_cutval(e);
	    if (save != ED_cutvalue(e))
		abort();
	}
    }
}

int check_ranks(void)
{
    int i, cost = 0;
    node_t *n;
    edge_t *e;

    for (n = GD_nlist(G); n; n = ND_next(n)) {
	for (i = 0; (e = ND_out(n).list[i]); i++) {
	    cost += (ED_weight(e)) * abs(LENGTH(e));
	    if (ND_rank(e->head) - ND_rank(e->tail) - ED_minlen(e) < 0)
		abort();
	}
    }
    fprintf(stderr, "rank cost %d\n", cost);
    return cost;
}

void checktree(void)
{
    int i, n = 0, m = 0;
    node_t *v;
    edge_t *e;

    for (v = GD_nlist(G); v; v = ND_next(v)) {
	for (i = 0; (e = ND_tree_out(v).list[i]); i++)
	    n++;
	if (i != ND_tree_out(v).size)
	    abort();
	for (i = 0; (e = ND_tree_in(v).list[i]); i++)
	    m++;
	if (i != ND_tree_in(v).size)
	    abort();
    }
    printf("%d %d %d\n", Tree_edge.size, n, m);
}

void check_fast_node(node_t * n)
{
    node_t *nptr;
    nptr = GD_nlist(n->graph);
    while (nptr && nptr != n)
	nptr = ND_next(nptr);
    assert(nptr != NULL);
}

static node_t *checkdfs(node_t * n)
{
    int i;
    edge_t *e;
    node_t *w,*x;

    if (ND_mark(n))
	return 0;
    ND_mark(n) = TRUE;
    ND_onstack(n) = TRUE;
    for (i = 0; (e = ND_out(n).list[i]); i++) {
	w = e->head;
	if (ND_onstack(w)) {
	    fprintf(stderr, "cycle: last edge %x %s(%x) %s(%x)\n",
		e,n->name,n,w->name,w);
	    return w;
	}
	else {
	    if (ND_mark(w) == FALSE) {
		x = checkdfs(w);
		if (x) {
		    fprintf(stderr,"unwind %x %s(%x)\n",e,n->name,n);
		    if (x != n) return x;
		    fprintf(stderr,"unwound to root\n");
		    fflush(stderr);
		    abort();
		    return 0;
		}
	    }
	}
    }
    ND_onstack(n) = FALSE;
    return 0;
}

void check_cycles(graph_t * g)
{
    node_t *n;
    for (n = GD_nlist(g); n; n = ND_next(n))
	ND_mark(n) = ND_onstack(n) = FALSE;
    for (n = GD_nlist(g); n; n = ND_next(n))
	checkdfs(n);
}
#endif				/* DEBUG */
