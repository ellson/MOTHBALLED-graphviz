#include "newdot.h"

/* given:
 *		ND_rank(v)	 		int			integer level assignments
 *		ND_ranksize(v)  int			number of levels (>= 1)
 *  	ED_xpenalty(e) 	float 	crossing penalty factor
 * find:
 *		ND_pos(v)				coord		
 *		ED_pos(e)				coord*
 *
 *		todo: graph, edge labels, flat edges
 */

/* internal graph variables:
 * user graph
 * 	  ND_cluster(v) 	graph*	lowest containing cluster
 * 		GD_model(g)			graph*	model graph
 *		GD_parent(g)		graph*	parent cluster
 *		GD_level				int			distance from layout root
 *
 * model graph objects
 *		GD_usergraph(g)	graph*		original graph or one of its clusters
 *		GD_minrank(g)		int				lowest rank index incl. external edges
 *		GD_maxrank(g)		int				highest rank index incl. external edges
 *		GD_minlocalrank(g)		int	lowest rank index of internal nodes
 *		GD_maxlocalrank(g)		int	highest rank index of internal nodes
 *		GD_rank(g)			rank_t[]
 *		GD_repdict(g)		Dict_t*		local representatives of user objects
 *		ND_component(v) int				connected component number
 *		ND_order(v)			int				order within rank
 *		ND_cluster(v)		graph*		for EXTNODE, its (lowest) user cluster
 */

/* TODO
	where do we use port.defined?
 */

/*** basic user<->model association maps ***/
/* determine canonical order of n0, n1 */
static void getlowhigh(Agnode_t **n0, Agnode_t **n1)
{
	Agnode_t	*temp;
	int				d;
	d =  ND_rank(*n0) - ND_rank(*n1);
	if ((d < 0) || ((d == 0) && ((*n0)->id > (*n1)->id)))
		{temp = *n0; *n0 = *n1; *n1 = temp;}
}

static int repkeycmp(Dt_t *d, void *arg0, void *arg1, Dtdisc_t *disc)
{
	void *key0 = ((repkey_t*)arg0)->key;
	void *key1 = ((repkey_t*)arg1)->key;
	int		rv;
	switch(agobjkind(key0)) {
	case AGNODE:
		rv = ((Agnode_t*)key0)->id - ((Agnode_t*)key1)->id;
		break;
	case AGEDGE:
		rv = ((Agedge_t*)key0)->id - ((Agedge_t*)key1)->id;
		break;
	case AGRAPH:
		/* in libgraph we don't seem to have graph ids, so use pointer */
		rv = (unsigned long)key0 - (unsigned long) key1;
		break;
	default:
		rv = 0;
	}
	return rv;
}

static Dtdisc_t Repdisc = {
	0,			/* pass whole object as key */
	0,			/* key size and type */
	-1,			/* link offset */
	(Dtmake_f)0,
	(Dtfree_f)0,
	(Dtcompar_f) repkeycmp,
	(Dthash_f)0,
	(Dtmemory_f)0,
	(Dtevent_f)0
};

static Dict_t *repdict(Agraph_t *model)
{
	Dict_t	*dict;

	dict = GD_repdict(model);
	if (!dict) dict = GD_repdict(model) = dtopen(&Repdisc,Dttree);
	return dict;
}

static rep_t association(Agraph_t *model, void *obj)
{
	Dict_t *dict;
	repkey_t	key, rv, *p;

	dict = repdict(model);
	key.key = obj;		/* i hate when other people code like this */
	if ((p = dtsearch(dict,&key))) rv = *p;
	else {rv.val.type = 0; rv.val.p = 0;}
	return rv.val;
}

static void associate(Agraph_t *model, void *key, rep_t val)
{
	Dict_t		*dict;
	repkey_t	*newelt;

	assert(association(model,key).type == 0);
	dict = repdict(model);
	newelt = NEW(repkey_t);
	newelt->key = key;
	newelt->val = val;
	dtinsert(dict,newelt);
}

typedef struct component_s {
	int		n;
	node_t	**root;
	int r;
} component_t;



/* from level.c - eventually clean this up. */
static int is_a_cluster(Agraph_t *g)
{
	return ((g == g->root) || (!strncasecmp(g->name,"cluster",7)));
}

/* find the cluster of n that is an immediate child of g */
static Agraph_t *subclustof(Agnode_t *n, Agraph_t *g)
{
	Agraph_t		*rv;
	for (rv = ND_cluster(n); rv && (GD_parent(rv) != g); rv = GD_parent(rv));
	return rv;
}

static void *T_array(int low, int high, int size)
{
	char	*rv;

	rv = calloc((high - low + 1),size);
	rv = rv - (low * size);
	return rv;
}

static Agnode_t **nodearray(int low, int high)
{ return (Agnode_t**) T_array(low,high,sizeof(Agnode_t*)); }

static Agedge_t **edgearray(int low, int high)
{ return (Agedge_t**) T_array(low,high,sizeof(Agedge_t*)); }

static int *intarray(int low, int high)
{ return (int*) T_array(low,high,sizeof(int)); }

/* internal functions for model graph construction and maintenance */
static vpath_t *newpath(Agraph_t *model, Agnode_t *u, int low, Agnode_t *v, int high)
{
	vpath_t		*path;
	int				i;
	path = NEW(vpath_t);
	path->v = nodearray(low, high);
	path->e = edgearray(low, high);
	for (i = low; i <= high; i++) {
		if ((i == low) && u) path->v[i] = u;
		else if ((i == high) && v) path->v[i] = v;
		else path->v[i] = agnode(model,0);
		if (i > low) path->e[i-1] = agedge(model,path->v[i-1],path->v[i]);
	}
	return path;
}

/* a given edge can have several other edges (forward or backward)
   between the same endpoints.  here, we choose one of these to be
	 the canonical representative of those edges. */
static Agedge_t* canonical_edge(Agedge_t* e)
{
	Agraph_t	*g;
	Agedge_t	*canon;

	g = e->tail->graph;
	if (ND_rank(e->head) > ND_rank(e->tail))
		canon = agfindedge(g,e->tail,e->head);
	else {
		if 
	}
}

static void model_edge(Agraph_t *model, Agedge_t *orig)
{
	Agedge_t	*e;
	Agnode_t	*low, *high, *u, *v;
	port_t		lowport, highport;
	vpath_t		*path;
	rep_t			rep;
	int				i;

	rep = association(model,orig);
	if (rep.type == 0) {
		low = orig->tail; high = orig->head;
		getlowhigh(&low,&high);
		u = association(model,low).p; assert(u);
		v = association(model,high).p; assert(v);
		path = newpath(model,u,ND_rank(low),v,ND_rank(high));
		rep.type = PATH;
		rep.p = path;
		associate(model,orig,rep);
	}
	else path = rep.p;

	/* merge the attributes of orig */
	for (i = path->low; i < path->high; i++) {
		e = path->e[i];
		ED_xpenalty(e) += ED_xpenalty(orig);
		ED_weight(e) += ED_weight(orig);
	}

	/* deal with ports.  note that ends could be swapped! */
	if (ND_rank(orig->tail) <= ND_rank(orig->head)) {
		lowport = ED_tailport(orig);
		highport = ED_headport(orig);
	}
	else {
		highport = ED_tailport(orig);
		lowport = ED_headport(orig);
	}
	if (lowport.defined)
		path->avgtailport = ((path->weight * path->avgtailport) + ED_weight(orig) * lowport.p.x) / (path->weight + ED_weight(orig));
	if (highport.defined)
		path->avgheadport = ((path->weight * path->avgheadport) + ED_weight(orig) * highport.p.x) / (path->weight + ED_weight(orig));
	path->weight += ED_weight(orig);
}

/* bind/construct representative of an internal node in a model graph */
static rep_t model_intnode(Agraph_t *model, Agnode_t *orig)
{
	int				nr;
	rep_t			rep;
	Agnode_t	*v;
	
	rep = association(model,orig);
	if (rep.type) return rep;
	
	nr = ND_ranksize(orig);
	if (nr <= 1) {			/* simple case */
		v = rep.p = agnode(model,orig->name);
		rep.type = NODE;
		ND_rank(v) = ND_rank(orig);
	}
	else {							/* multi-rank node case */
		rep.p = newpath(model,NILnode,ND_rank(orig),NILnode,ND_rank(orig)+nr-1);
		rep.type = TALLNODE;
	}
	associate(model,orig,rep);
	return rep;
}

/* bind/construct representative of an external endpoint to a model graph */
static rep_t model_extnode(Agraph_t *model, Agnode_t *orig)
{
	Agnode_t	*v;
	rep_t		rep;

	rep = association(model,orig);
	if (rep.type) return rep;

	/* assume endpoint is represented by one node, even if orig is multi-rank */
	rep.p = v = agnode(model,orig->name);
	rep.type = EXTNODE;
	ND_rank(v) = ND_rank(orig);	/* should be ND_rank(orig)+ranksize(orig)? */
	associate(model,orig,rep);
	return rep;
}

/* bind/construct representative of an internal cluster of a model graph */
static rep_t model_clust(Agraph_t *model, Agraph_t *origclust)
{
	rep_t		rep;

	rep = association(model,origclust);
	if (rep.type) return rep;

	rep.p = newpath(model,NILnode,GD_minrank(origclust),NILnode,GD_maxrank(origclust));
	rep.type = SKELETON;
	associate(model,origclust,rep);
	return rep;
}

#ifdef NOTDEF
	/* i think we ditched this because we assume nodes are always
	built before edges */
/* helper functions for model_edge */
static rep_t bindnode(Agraph_t *model, Agnode_t *orignode)
{
	rep_t	rep;
	if (agcontains(GD_originalgraph(model),orignode))
		rep = model_intnode(model,orignode);
	else
		rep = model_extnode(model,orignode);
	return rep;
}
#endif

static int leftmost(Agraph_t *model, int r) { return 0; }
static int rightmost(Agraph_t *model, int r) {return GD_rank(model)[r].n - 1;}

static void flat_edges(Agraph_t *clust)
{
#ifdef NOTDEF
	for (n = agfstnode(clust); n; n = agnxtnode(clust)) {
		for (e = agfstedge(root,n); e; e = agnxtedge(root,e,n)) {
		}
	}
	ordered_edges();
#endif
}

static void search_component(Agraph_t *g, Agnode_t *n, int c)
{
	Agedge_t	*e;
	ND_component(n) = c;
	for (e = agfstout(g,n); e; e = agnxtout(g,e))
		if (ND_component(e->head) < 0)
			search_component(g,e->head,c);
	for (e = agfstin(g,n); e; e = agnxtin(g,e))
		if (ND_component(e->tail) < 0)
			search_component(g,e->tail,c);
}

static int ND_comp_cmpf(const void *arg0, const void *arg1)
{
	return ND_component(*(Agnode_t**)arg0) - ND_component(*(Agnode_t**)arg1);
}

static component_t build_components(Agraph_t *g, boolean down)
{
	component_t	rv;
	node_t	*n;
	int		r, rootcnt, compcnt, deg;

	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		ND_component(n) = -1;	/* initialize to unknown component */

	compcnt = 0; rootcnt = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) { 
		/* set priority for subsequent BFS to install nodes, and record roots */
		if (down) deg = ND_indeg(n);
		else deg = ND_outdeg(n);
		ND_priority(n) = deg;
		if (deg == 0) rootcnt++;
		/* count and mark components */
		if (ND_component(n) < 0) search_component(g,n,compcnt++);
	}

	rv.n = compcnt;
	rv.r = rootcnt;
	rv.root = N_NEW(rv.r,Agnode_t*);
	r = 0;
	/* install roots in root list */
	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		if (ND_priority(n) == 0) rv.root[r++] = n;
	/* sort root list so components are contiguous */
	qsort(rv.root,rv.n,sizeof(node_t*),ND_comp_cmpf);
	return rv;
}

static void install(Agraph_t *g, Agnode_t *n)
{
	int				rank;
	rank_t		*r;

	rank = ND_rank(n);
	r = &GD_rank(g)[rank];
	r->v[r->n] = n;
	ND_order(n) = r->n++;
}

/* 
 populates rank lists of g.  there are some key details:
 1) the input graph ordering must be respected (in left to right initialization)
 2) connected components are separated and marked with indices
 3) series-parallel graphs (includes trees, obviously) must not have crossings
*/
static void build_ranks(Agraph_t *g, boolean down)
{
	queue			*q;
	component_t c;
	int				r;
	Agnode_t	*n;
	Agedge_t	*e;

	c = build_components(g, down);

	/* process each each component */
	q = new_queue(agnnodes(g)+1);
	for (r = 0; r < c.r; r++) {
		enqueue(q,c.root[r]);
		if ((r + 1 >= c.r)||(ND_component(c.root[r])!=ND_component(c.root[r+1]))) {
			while ((n = dequeue(q))) {
				install(g,n);
					if (down) {
						for (e = agfstout(g,n); e; e = agnxtout(g,e))
							if (--ND_priority(e->head) == 0) enqueue(q,e->head);
					}
					else {
						for (e = agfstin(g,n); e; e = agnxtin(g,e))
							if (--ND_priority(e->tail) == 0) enqueue(q,e->head);
					}
			}
		}
	}
	free_queue(q);
}

/* this predicate indicates if mincross should be run on this cluster */
static boolean run(Agraph_t *mg)
{
	if (GD_pass(mg) > GD_maxpass(mg)) return FALSE;
	if (GD_pass(mg) - GD_lastwin(mg) > GD_mintry(mg)) return FALSE;
	GD_pass(mg) = GD_pass(mg) + 1;
	return TRUE;
}

static int presort_cmpf(const void *arg0, const void *arg1)
{
	Agnode_t	*n0, *n1;
	Agraph_t	*c0, *c1;

	n0 = *(Agnode_t**)arg0;
	n1 = *(Agnode_t**)arg1;
	c0 = ND_cluster(n0);
	c1 = ND_cluster(n1);
	if (c0 == c1) return 0;
	assert(ND_rank(n0) == ND_rank(n1));
	n0 = GD_skel(c0)->v[ND_rank(n0)];
	n1 = GD_skel(c1)->v[ND_rank(n1)];
	return ND_order(n0) - ND_order(n1);
}

static void presort(Agraph_t *ug)
{
	int		r;
	int		i;
	Agraph_t	*mg;

	if (ug == ug->root) return;
	mg = GD_model(ug);
	for (r = GD_minrank(mg); r <= GD_maxrank(mg); r++) {
		qsort(GD_rank(mg)[r].v,GD_rank(mg)[r].n,sizeof(Agnode_t*),presort_cmpf);
		for (i = leftmost(mg,r); i < rightmost(mg,r); i++)
			ND_order(GD_rank(mg)[r].v[i]) = i;
	}
}

/* sets ports that represent connections to subclusters */
static void subclustports(Agraph_t *ug)
{
	Agraph_t	*model, *clustmodel;
	Agnode_t	*x;
	Agedge_t	*e;
	vpath_t		*p;
	Dict_t		*d;
	double		frac;

	/* walk all the paths */
	model = GD_model(ug);
	d = repdict(model);
	for (p = dtfirst(d); p; p = dtnext(d,p)) {
		if ((ND_type(p->v[p->low])) == NODETYPE_CNODE) {
			x = p->v[p->low];
			clustmodel = GD_model(ND_cluster(x));
			frac = (ND_order(x) + 1) / GD_rank(clustmodel)[ND_rank(x)].n;
			e = p->e[p->low];
			ED_tailport(e).p.x = 2 * PORTCLAMP * (frac - 0.5) + p->tailport;
		}
		if ((ND_type(p->v[p->high])) == NODETYPE_CNODE) {
			x = p->v[p->high];
			clustmodel = GD_model(ND_cluster(x));
			frac = (ND_order(x) + 1) / GD_rank(clustmodel)[ND_rank(x)].n;
			e = p->e[p->high-1];
			ED_headport(e).p.x = 2 * PORTCLAMP * (frac - 0.5) + p->headport;
		}
	}
}

static void mincross_clust(Agraph_t *ug)
{
	Agraph_t	*g;
	g = GD_model(ug);
	if (run(g)) {
		presort(ug);		/* move the external nodes */
		subclustports(ug);
		do {
			mincross_sweep(g,GD_pass(g)%2,GD_pass(g)%4<2);
		} while (run(g));
		transpose_sweep(g,TRUE);
		restorebest(g);
	}
}

static void globalopt(Agraph_t *root)
{
	Agraph_t	*g;
	rank_t		*glob;

	g = GD_model(root);
	glob = globalize(root,g);
	GD_rank(g) = glob;
	fprintf(stderr,"%s: %d crossings\n",root->name,crossings(g));
}

/* this assumes one level per node - no mega-nodes */
static void apply_model(Agraph_t *ug)
{
	Agnode_t *un;
	for (un = agfstnode(ug); un; un = agnxtnode(ug,un)) 
			ND_order(un) = ND_order(ND_rep(un));
}

/* this is a first cut at a top-level planner.  it's lame. */
static void rec_cluster_run(Agraph_t *ug)
{
	Agraph_t	*subg;

	if (is_a_cluster(ug)) mincross_clust(ug);
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(ug,subg))
		rec_cluster_run(subg);
	if (is_a_cluster(ug)) mincross_clust(ug);
}

/* this is the top level mincross entry point */
void dot_mincross(Agraph_t *user)
{
	rec_cluster_init(user);
	rec_cluster_run(user);
	globalopt(user);
	apply_model(user);
}

static void invalidate(Agraph_t *g, int rank)
{
	if (rank > GD_minrank(g)) GD_rank(g)[rank-1].crossing_cache.valid = FALSE;
	if (rank > GD_minrank(g)) GD_rank(g)[rank-1].candidate = TRUE;
	if (rank < GD_maxrank(g)) GD_rank(g)[rank+1].candidate = TRUE;
}

/* swaps two nodes in the same level */
static void exchange(Agraph_t *g, Agnode_t *u, Agnode_t *v)
{
	rank_t	*r;
	int			ui,vi,rank;

	assert(ND_rank(u) == ND_rank(v));
	rank = ND_rank(u);
	r = &GD_rank(g)[rank];
	ui = ND_order(u);
	vi = ND_order(v);
	ND_order(v) = ui;
	ND_order(u) = vi;
	r->v[ND_order(u)] = u;
	r->v[ND_order(v)] = v;
	r->crossing_cache.valid = FALSE;
	r->changed = TRUE;
	r->candidate = TRUE;	/* old dot had this.  i have qualms. sn */
	invalidate(g,rank);
}

int transpose_onerank(Agraph_t* g, int r, boolean reverse)
{
	int     i,c0,c1,rv;
	node_t  *v,*w;

	rv = 0;
	GD_rank(g)[r].candidate = FALSE;
	for (i = leftmost(g,r); i < rightmost(g,r); i++) {
		v = GD_rank(g)[r].v[i];
		w = GD_rank(g)[r].v[i+1];
		assert (ND_order(v) < ND_order(w));
		if (left2right(g,v,w)) continue;
		c0 = c1 = 0;
		if (r > GD_minrank(g)) {
			c0 += in_cross(v,w);
			c1 += in_cross(w,v);
		}
		if (r < GD_maxrank(g)) {
			c0 += out_cross(v,w);
			c1 += out_cross(w,v);
		}
		if ((c1 < c0) || ((c0 > 0) && reverse && (c1 == c0))) {
			exchange(g,v,w);
			rv += (c0 - c1);
		}
	}
	return rv;
}

static void transpose_sweep(Agraph_t* g, int reverse)
{
    int     r,delta;

	for (r = GD_minrank(g); r <= GD_maxrank(g); r++)
		GD_rank(g)[r].candidate = TRUE;
    do {
			delta = 0;
			for (r = GD_minrank(g); r <= GD_maxrank(g); r++)
				if (GD_rank(g)[r].candidate) delta += transpose_onerank(g,r,reverse);
    }
		while (delta >= 1);
		/* while (delta > crossings(g)*(1.0 - Convergence));*/
}

static void mincross_sweep(Agraph_t* g, int dir, boolean reverse)
{
    int     r,other,low,high,first,last;
    int     hasfixed;

		low = GD_minrank(g);
		high = GD_maxrank(g);
		if (dir == 0) return;
		if (dir > 0)  { first = low + 1; last = high; dir = 1;}		/* down */
		else				  { first = high - 1; last = low; dir = -1;}    /* up */

    for (r = first; r != last + dir; r += dir) {
        other = r - dir;
        hasfixed = medians(g,r,other);
        reorder(g,r,reverse,hasfixed);
    }
    transpose_sweep(g,NOT(reverse));
	savebest(g);
}


static int left2right(Agraph_t *g, node_t *v, node_t *w)
{
    int         rv;

#ifdef NOTDEF
    adjmatrix_t *M;
    M = GD_rank(g)[ND_rank(v)].flat;
    if (M == NULL) rv = FALSE;
    else {
        if (GD_flip(g)) {node_t *t = v; v = w; w = t;}
        rv = ELT(M,flatindex(v),flatindex(w));
    }
#else
		rv = FALSE;
#endif
    return rv;
}

static void build_flat_graphs(Agraph_t *g)
{
}

static int out_cross(node_t *v, node_t *w)
{
  register edge_t *e1,*e2;
  register int  inv, cross = 0,t;

  for (e2 = agfstout(w->graph,w); e2; e2 = agnxtout(w->graph,e2)) {
		register int cnt = ED_xpenalty(e2);
		inv = ND_order(e2->head);
		for (e1 = agfstout(v->graph,v); e1; e1 = agnxtout(v->graph,e1)) {
			t = ND_order(e1->head) - inv;
			if ((t > 0) || ((t == 0) && (ED_headport(e1).p.x > ED_headport(e2).p.x)))
				cross += ED_xpenalty(e1) * cnt;
    }
	}
	return cross;
}
                                                                                
static int in_cross(node_t *v,node_t *w)
{
  register edge_t *e1,*e2;
  register int inv, cross = 0, t;
                                                                                
  for (e2 = agfstin(w->graph,w); e2; e2 = agnxtin(w->graph,e2)) {
		register int cnt = ED_xpenalty(e2);
		inv = ND_order(e2->tail);
		for (e1 = agfstin(v->graph,v); e1; e1 = agnxtin(v->graph,e1)) {
			t = ND_order(e1->tail) - inv;
			if ((t > 0) || ((t == 0) && (ED_tailport(e1).p.x > ED_tailport(e2).p.x)))
				cross += ED_xpenalty(e1) * cnt;
    }
	}
	return cross;
}

static int int_cmpf(const void *arg0, const void *arg1)
{
	return *(int*)arg0 - *(int*)arg1;
}


/* 8 is the number of bits in port.order, an unsigned char */
#define VAL(node,port) (((node)->u.order << 8)  + (port).order)

/*
 * defines ND_sortweight of each node in r0 w.r.t. r1
 * returns...
 */
static boolean medians(Agraph_t *g, int r0, int r1)
{
	static int *list;
	static int list_extent;
	int     i,j,lm,rm,lspan,rspan;
	node_t  *n,**v;
	edge_t  *e;
	boolean hasfixed = FALSE;

	if (list_extent < GD_maxinoutdeg(g->root)) {
		list_extent = GD_maxinoutdeg(g->root);
		if (!list) list = realloc(list,sizeof(list[0])*list_extent);
		else list = realloc(list,sizeof(list[0])*list_extent);
	}
	v = GD_rank(g)[r0].v;
	for (i = leftmost(g,r0); i <= rightmost(g,r0); i++) {
		n = v[i]; j = 0;
		if (r1 > r0) for (e = agfstout(g,n); e; e = agnxtout(g,e))
			{if (ED_xpenalty(e) > 0) list[j++] = VAL(e->head,ED_headport(e));}
		else for (e = agfstin(g,n); e; e = agnxtin(g,e))
			{if (ED_xpenalty(e) > 0) list[j++] = VAL(e->tail,ED_tailport(e));}
		switch(j) {
			case 0:
				ND_sortweight(n) = -1;		/* no neighbor - median undefined */
				break;
			case 1:
				ND_sortweight(n) = list[0];
				break;
			case 2:
				ND_sortweight(n) = (list[0] + list[1])/2;
				break;
			default:
				qsort(list,j,sizeof(int),int_cmpf);
				if (j % 2) ND_sortweight(n) = list[j/2];
				else {
					/* weighted median */
					rm = j/2;
					lm = rm - 1;
					rspan = list[j-1] - list[rm];
					lspan = list[lm] - list[0];
					if (lspan == rspan)
						ND_sortweight(n) = (list[lm] + list[rm])/2;
					else {
						int w = list[lm]*rspan + list[rm]*lspan;
						ND_sortweight(n) = w / (lspan + rspan);
					}
				}
		}
	}
#ifdef NOTDEF
	/* this code was in the old mincross */
	for (i = 0; i < GD_rank(g)[r0].n; i++) {
		n = v[i];
		if ((ND_out(n).size == 0) && (ND_in(n).size == 0))
			hasfixed |= flat_sortweight(n);
	}
#endif
	return hasfixed;
}

static void reorder(graph_t *g, int r, boolean reverse, boolean hasfixed)
{
	boolean changed, muststay;
	node_t  **vlist, **lp, **rp, **ep;
	int			i;

	changed = FALSE;
	vlist = GD_rank(g)[r].v;
	ep = &vlist[rightmost(g,r)];
	
	for (i = leftmost(g,r); i <= rightmost(g,r); i++) {
		lp = &vlist[leftmost(g,r)];
		/* find leftmost node that can be compared */
		while ((lp < ep) && (ND_sortweight(*lp) < 0)) lp++;
		if (lp >= ep) break;
		/* find the node that can be compared */
		muststay = FALSE;
		for (rp = lp + 1; rp < ep; rp++) {
			if (left2right(g,*lp,*rp)) { muststay = TRUE; break; }
			if (ND_sortweight(*rp) >= 0) break;	/* weight defined; it's comparable */
		}
		if (rp >= ep) break;
		if (muststay == FALSE) {
			register int    p1 = ND_sortweight(*lp);
			register int    p2 = ND_sortweight(*rp);
			if ((p1 > p2) || ((p1 == p2) && (reverse))) {
				exchange(g,*lp,*rp);
				changed = TRUE;
			}
		}
		lp = rp;
		if ((hasfixed == FALSE) && (reverse == FALSE)) ep--;
	}
                                                                                
	if (changed) {
		GD_rank(g)[r].changed = TRUE;
		GD_rank(g)[r].crossing_cache.valid = FALSE;
		if (r > 0) GD_rank(g)[r-1].crossing_cache.valid = FALSE;
		if (r + 1 > GD_rank(g)[r+1].n) GD_rank(g)[r-1].crossing_cache.valid = FALSE;
	}
}

static void savebest(graph_t *g)
{
	int		nc;
	Agnode_t	*n;

	nc = crossings(g);
	if (nc < GD_bestcrossings(g)) {
		for (n = agfstnode(g); n; n = agnxtnode(g,n))
			ND_saveorder(n) = ND_order(n);
		GD_bestcrossings(g) = nc;
		GD_lastwin(g) = GD_pass(g);
	}
}

static int ND_order_cmpf(const void *arg0, const void *arg1)
{
	return ND_order(*(Agnode_t**)arg0) - ND_order(*(Agnode_t**)arg1);
}

static void restorebest(graph_t *g)
{
	Agnode_t	*n;
	int			i;

	for (i = GD_minrank(g); i <= GD_maxrank(g); i++) 
		GD_rank(g)[i].changed = FALSE;
	for (n = agfstnode(g); n; n = agnxtnode(g,n))  {
		if (ND_order(n) != ND_saveorder(n)) {
			invalidate(g,ND_rank(n));
			GD_rank(g)[i].changed = TRUE;
			ND_order(n) = ND_saveorder(n);
		}
	}
	for (i = GD_minrank(g); i <= GD_maxrank(g); i++)  {
		if (GD_rank(g)[i].changed)
			qsort(GD_rank(g)[i].v,GD_rank(g)[i].n,sizeof(Agnode_t*),ND_order_cmpf);
	}
}

static int crossings(graph_t *g)
{
		int		i, rv;

		rv = 0;
		for (i = GD_minrank(g); i < GD_maxrank(g); i++) {
			rv += crossings_below(g,i);
		}
		return rv;
}

/* build the global (flat) graph of the universe.  this is 'flat'
in the sense that there is one data structure for the entire graph
(not 'flat' in the sense of flat edges within the same level.)
*/
static rank_t *globalize(Agraph_t *user, Agraph_t *topmodel)
{
	rank_t	*globrank;
	int		minr,maxr,r;

	/* setup bookkeeping */
	interclusterpaths(user, topmodel);

	/* allocate global ranks */
	minr = GD_minrank(topmodel);
	maxr = GD_maxrank(topmodel);
	globrank = T_array(minr,maxr,sizeof(rank_t));
	countup(user,globrank);
	for (r = minr; r <= maxr; r++) {
		globrank[r].v = N_NEW(globrank[r].n+1,Agnode_t*);	/* NIL at end */
		globrank[r].n = 0;	/* reset it */
	}

	/* installation */
	for (r = minr; r <= maxr; r++)
		installglob(user,topmodel,globrank,r);

	removejunk(user, topmodel);
	reconnect(user, topmodel);

	/* optimization */
	return globrank;
}

static void countup(Agraph_t *g, rank_t *globr)
{
	Agnode_t	*n;
	Agedge_t	*e;
	int				r0, r1, low, high, i;

	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
			for (i = 0; i < ND_ranksize(n); i++)
				globr[ND_rank(n)+i].n += 1;
			for (e = agfstout(g,n); e; e = agnxtout(g,e)) {
				r0 = ND_rank(e->tail);
				r1 = ND_rank(e->head);
				low = MIN(r0,r1);
				high = MAX(r0,r1);
				for (i = low + 1; i < high; i++)
					globr[i].n += 1;
			}
	}
}

static void rec_model_subclusts(Agraph_t *model, Agraph_t *user)
{
	Agraph_t	*subg;

	if (is_a_cluster(user))
		(void) model_clust(model,user);
	/* note there can be non-cluster subgraphs that contain lower clusters */
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(ug,subg))
		rec_model_subclusts(model,subg);
}

static void cluster_contents(Agraph_t *ug)
{
	Agraph_t	*mg;
	char *name;

	assert(is_a_cluster(ug));
	assert(GD_model(ug) == 0);

	name = malloc(strlen(ug->name)+10);
	sprintf(name,"model_%s",ug->name);
	GD_model(ug) = mg = agopen(name,AGDIGRAPHSTRICT);
	free(name);

	/* install internal nodes */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		if (ND_cluster(n) == ug) model_intnode(mg,n);
	}

	/* install cluster skeletons */
	rec_model_subclusts(mg,ug);

	/* install external edge endpoints */
	root = GD_layoutroot(ug);
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstout(root,n); e; e = agnxtout(root,e)) {
			if (!agcontains(ug,e->tail))
				model_extnode(mg,e->tail);
			if (!agcontains(ug,e->head))
				model_extnode(mg,e->head);
		}
	}

	/* install edges */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n))
		for (e = agfstout(root,n); e; e = agnxtout(root,e))
			model_edge(mg,e);		/* amazing if this is all it takes */
}

static void cluster_init(Agraph_t *ug)
{
	cluster_contents(ug);
	flat_edges(ug);
}

static void rec_cluster_init(Agraph_t *ug)
{
	Agraph_t	*subg;

	if (is_a_cluster(ug)) cluster_init(ug);
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(ug,subg))
		rec_cluster_init(subg);
}
