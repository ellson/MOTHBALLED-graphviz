#include "newdot.h"

/* given:
 *  each node and cluster has a unique containing cluster.
 *	ND_level is the integer level of each node.
 *  ND_cluster is the lowest containing cluster of each node.
 *  ED_xpenalty is the crossing weight for an edge.
 * find:
 *  GD_rank - the levels of the graph
 *  ND_order - the global order of each node
 */

typedef struct component_s {
	int		n;
	node_t	**root;
	int r;
} component_t;

static Agraph_t *subclustof(Agnode_t *n, Agraph_t *clust);
static void *T_array(int low, int high, int size);
static Agnode_t **nodearray(int low, int high);
static int *intarray(int low, int high);
static Agraph_t *cluster_model(Agraph_t *clust);
static void flat_edges(Agraph_t *clust);
static void search_component(Agraph_t *g, Agnode_t *n, int c);
static int ND_comp_cmpf(const void *arg0, const void *arg1);
static component_t build_components(Agraph_t *g, boolean down);
static void install(Agraph_t *g, Agnode_t *n);
static void build_ranks(Agraph_t *ug, boolean down);
static void cluster_init(Agraph_t *userclust);
static void rec_cluster_init(Agraph_t *userclust);
static boolean run(Agraph_t *g);
static void mincross_clust(Agraph_t *clust);
static void globalopt(Agraph_t *ug);
static void rec_cluster_run(Agraph_t *ug);
static void exchange(Agraph_t *g, Agnode_t *u, Agnode_t *v);
static void transpose_sweep(graph_t* g, int reverse);
static void mincross_sweep(graph_t* g, int dir, boolean reverse);
static int left2right(graph_t *g, node_t *v, node_t *w);
static void build_flat_graphs(graph_t *g);
static boolean medians(graph_t *g, int r0, int r1);
static void reorder(graph_t *g, int r, boolean reverse, boolean hasfixed);
static void savebest(graph_t *g);
static void restorebest(graph_t *g);
static int crossings(graph_t *g);
static int in_cross(node_t *v,node_t *w);
static int out_cross(node_t *v,node_t *w);
static rank_t *globalize(Agraph_t *user, Agraph_t *model);
static void installglob(Agraph_t *user, Agraph_t *fromgraph, rank_t *globr, int r);
static void countup(Agraph_t *g, rank_t *globr);

static Agnode_t *model_getrep(Agraph_t *ug, Agnode_t *n, Agedge_t *e);
static Agraph_t *universegraph(Agraph_t *ug);
static vpath_t *model_skel(Agraph_t *uclust, Agraph_t *model);
static void clusterchains(Agraph_t *model, Agraph_t *ug);
static void removejunk(Agraph_t *ug, Agraph_t *topmodel);
static void reconnect(Agraph_t *ug, Agraph_t *topmodel);

/* from level.c - eventually clean this up. */
static int is_a_cluster(graph_t *g)
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

static Agnode_t *model_realnode(Agraph_t *modelgraph, Agnode_t *orig)
{
	Agnode_t	*rep;
	/* this is OK because realnode can only be in one model graph/subgraph */
	rep = agnode(modelgraph,orig->name);
	ND_rank(rep) = ND_rank(orig);
	ND_cluster(rep) = modelgraph;
	ND_vrep(orig) = rep;
	ND_vrep(rep) = orig;
	ND_type(rep) = NODETYPE_REAL;
	/* other initializations may need to go here */
	return rep;
}

static Agnode_t *model_extnode(Agraph_t *modelgraph, Agnode_t *orig, Agedge_t *e)
{
	char name[1024];
	Agnode_t	*rep;

	sprintf(name,"ext_%s_%s",modelgraph->name,orig->name);
	/* can't be anonymous - needs to be repeatable */
	rep = agnode(modelgraph,name);
	ND_rank(rep) = ND_rank(orig);
	ND_cluster(rep) = modelgraph;
	ND_erep(rep) = e;
	ND_type(rep) = NODETYPE_XNODE;
	/* other initializations may need to go here */
	return rep;
}

static Agnode_t *model_vnode(Agraph_t *modelgraph, Agedge_t *longedge, int level)
{
	Agnode_t	*rep;
	rep = agnode(modelgraph,0);
	ND_rank(rep) = level;
	ND_cluster(rep) = modelgraph;
	ND_erep(rep) = longedge;
	ND_type(rep) = NODETYPE_VNODE;
	/* other initializations may need to go here */
	return rep;
}

static Agnode_t *model_cnode(Agraph_t *modelgraph, Agraph_t *clust, int level)
{
	Agnode_t	*rep;
	rep = agnode(modelgraph,0);
	ND_rank(rep) = level;
	ND_cluster(rep) = clust;
	ND_grep(rep) = clust;
	ND_type(rep) = NODETYPE_CNODE;
	/* other initializations may need to go here */
	return rep;
}

static Agedge_t *model_vedge(Agraph_t *modelgraph, Agnode_t *tx, Agnode_t *hx, Agedge_t *orig)
{
	Agedge_t	*e;

	e = agedge(modelgraph,tx,hx);
	ED_xpenalty(e) = VEDGE_PENALTY * ED_xpenalty(orig);
	ED_weight(e) = ED_weight(orig);
	return e;
}

static Agedge_t *model_cedge(Agraph_t *modelgraph, Agnode_t *tx, Agnode_t *hx)
{
	Agedge_t	*e;

	e = agedge(modelgraph,tx,hx);
	ED_xpenalty(e) = CEDGE_PENALTY;
	ED_weight(e) = 1;
	return e;
}

static void getlowhigh(Agnode_t **n0, Agnode_t **n1)
{
	Agnode_t	*temp;
	if (ND_rank(*n0) > ND_rank(*n1))
		{temp = *n0; *n0 = *n1; *n1 = temp;}
}

vpath_t *newpath(Agnode_t *u, Agnode_t *v)
{
	Agnode_t	*origlow = u, *orighigh = v;
	vpath_t		*p;

	getlowhigh(&origlow,&orighigh);
	p = NEW(vpath_t);
	p->key.tail = origlow;
	p->key.head = orighigh;
	p->low = ND_rank(origlow);
	p->high = ND_rank(orighigh);
	p->v = nodearray(p->low, p->high);
	p->e = edgearray(p->low, p->high);
	return p;
}

static int pathcmp(Dt_t *d, void *arg0, void *arg1, Dtdisc_t *disc)
{
	int		rv;
	if ((rv = ((vpath_t*)arg0)->key.tail - ((vpath_t*)arg1)->key.tail) == 0)
		rv = ((vpath_t*)arg0)->key.head - ((vpath_t*)arg1)->key.head;
	return rv;
}

static Dtdisc_t Vpathdisc = {
	0,			/* pass whole object as key */
	0,			/* key size and type */
	-1,			/* link offset */
	(Dtmake_f)0,
	(Dtfree_f)0,
	(Dtcompar_f) pathcmp,
	(Dthash_f)0,
	(Dtmemory_f)0,
	(Dtevent_f)0
};

static Dict_t *pathdict(Agraph_t *g)
{
	Dict_t	*dict;
	dict = GD_pathdict(g);
	if (!dict) dict = GD_pathdict(g) = dtopen(&Vpathdisc,Dttree);
	return dict;
}

static vpath_t *pathsearch(Agraph_t *g, Agnode_t *u, Agnode_t *v)
{
	vpath_t	*p,path;
	Dict_t	*dict;
	Agnode_t	*origlow = u, *orighigh = v;

	getlowhigh(&origlow, &orighigh);
	dict = pathdict(g);
	path.key.tail = origlow;
	path.key.head = orighigh;
	p = dtsearch(dict,&path);
	return p;
}

static void pathinsert(Agraph_t *g, vpath_t *p)
{
	Dict_t	*dict;
	dict = pathdict(g);
	p = dtinsert(dict,p);
}

vpath_t *model_path(Agraph_t *modelgraph, Agnode_t *tx, Agnode_t *hx, Agedge_t *orig)
{
	vpath_t	*p;
	Agnode_t	*origlow, *orighigh;
	int			i;

	getlowhigh(&tx,&hx);
	origlow = orig->tail; orighigh = orig->head;
	getlowhigh(&origlow,&orighigh);

	p = pathsearch(modelgraph,origlow,orighigh);
	if (!p) {
		p = newpath(origlow,orighigh);
		for (i = p->low; i <= p->high; i++) {
			if (i == p->low) p->v[i] = tx;
			else if (i == p->high) p->v[i] = hx;
			else p->v[i] = model_vnode(modelgraph,orig,i);
			if (i > p->low)
				p->e[i] = model_vedge(modelgraph,p->v[i-1],p->v[i],orig);
		}
		pathinsert(modelgraph,p);
	}
	return p;
}

/* get model node for an endpoint (can be external to the clust).  assumes
each node and cluster has a unique parent, and nodes have unique names.
clust and node are from a user graph, not an internal model graph. 
*/
static Agnode_t *model_getrep(Agraph_t *uclust, Agnode_t *un, Agedge_t *ue)
{
	Agraph_t	*model = GD_model(uclust);
	Agraph_t	*subclust;
	vpath_t		*skel;
	Agnode_t	*rep;

	if (agcontains(uclust,un)) {	/* internal */
		if (ND_cluster(un) == uclust) {	/* primitive */
			rep = ND_rep(un);
			if (!rep) {
				/* i don't believe this should ever really happen because
				all real nodes were already processed; consider this safety code  */
				rep = model_realnode(model,un);
			}
		}
		else {							/* in a subcluster */
			subclust = subclustof(un,uclust);
			skel = model_skel(model,subclust);
			rep = skel->v[ND_rank(un)];
		}
	}
	else { /* external */
		rep = model_extnode(model,un,ue);
	}
	return rep;
}

static vpath_t *model_skel(Agraph_t *model, Agraph_t *uclust)
{
	vpath_t		*skel;
	int				i;

	if (!((skel = GD_skel(uclust)))) {
		skel = GD_skel(uclust) = NEW(vpath_t);
		skel->key.tail = skel->key.head = NILnode;	/* skeletons aren't indexed */
		skel->low = GD_minrank(uclust);
		skel->high = GD_maxrank(uclust);
		skel->v = nodearray(skel->low,skel->high);
		skel->e = edgearray(skel->low,skel->high);
		for (i = skel->low; i <= skel->high; i++) {
			skel->v[i] = model_cnode(model,uclust,i);
			if (i > skel->low)
				skel->e[i - 1] = model_cedge(model,skel->v[i-1],skel->v[i]);
		}
	}
	return skel;
}

static void scangraph(Agraph_t *g)
{
	Agnode_t	*n;
	Agedge_t	*e;
	short			indeg, outdeg;

	if ((n = agfstnode(g)) == NILnode) return;

	model = universegraph(g);
	GD_minrank(g) = GD_maxrank(g) = ND_rank(n);
	GD_maxinoutdeg(g) = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		if (GD_maxrank(g) < ND_rank(n)) GD_maxrank(g) = ND_rank(n);
		if (GD_minrank(g) > ND_rank(n)) GD_minrank(g) = ND_rank(n);
		if (ND_indeg(n) == 0) {
			indeg = 0; for (e = agfstin(g,n); e; e = agnxtin(g,e)) indeg++;
			ND_indeg(n) = indeg;
		}
		if (ND_outdeg(n) == 0) {
			outdeg = 0; for (e = agfstout(g,n); e; e = agnxtout(g,e)) outdeg++;
			ND_outdeg(n) = outdeg;
		}
		GD_maxinoutdeg(g) = MAX(GD_maxinoutdeg(g),MAX(indeg,outdeg));
	}
}

/* Build a model graph for each cluster + its ext edges.
   The model is a subgraph of a 'universal graph' for all
	 the objects of a graph and its clusters.  This makes it
	 possible to merge the cluster (models) later.  Fortunately
	 each primitive node only belongs to one cluster.  On the
	 other hand, naming all the virtual objects uniquely is so much
	 useless overhead.  (This is one thing libAgraph handles reasonably
	 and that it might make sense to port back to libgraph.)

	 Note that the cluster models are not thesmselves nested subgraphs.
	 This is because we don't want internal objects of a cluster to
	 propagate to its enclosing clusters.
	 */
static Agraph_t *cluster_model(Agraph_t *ug)
{
	graph_t		*model,*universe;
	node_t		*n;
	edge_t		*e;
	node_t		*tx, *hx;
	int				minr,maxr,r,*ranksize;

	/* get model graph */
	if (GD_model(ug)) abort(); /* double initialization? not yet re-entrant. */
	universe = universegraph(ug);
	model = GD_model(ug) = agsubg(universe,ug->name);

	/* create nodes explicitly.  this copies the search order of original nodes
	   and ensures singletons (including those in subclusters) are created */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		if (ND_cluster(n) == ug) (void) model_realnode(model,n);
		else clusterchains(model,subclustof(n,ug));
	}

	/* edges (including external ones) */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstedge(ug->root,n); e; e = agnxtedge(ug->root,e,n)) {
			tx = model_getrep(ug,e->tail,e);
			hx = model_getrep(ug,e->head,e);
			if (tx == hx) continue;
			/* skip edges that are within some subcluster */
			if ((ND_type(tx) == NODETYPE_CNODE) && (ND_type(hx) == NODETYPE_CNODE) &&
				(ND_cluster(tx) == ND_cluster(hx))) continue; 
			(void) model_path(model,tx,hx,e);
		}
	}

	/* initialize storage for ranks in the model; nodes are installed later */
	GD_minrank(model) = GD_minrank(ug);
	GD_maxrank(model) = GD_maxrank(ug);
	minr = GD_minrank(model);
	maxr = GD_maxrank(model);
	ranksize = intarray(minr,maxr);
	for (n = agfstnode(model); n; n = agnxtnode(model,n))
		ranksize[ND_rank(n)]++;
	GD_rank(model) = T_array(minr,maxr,sizeof(rank_t));
	for (r = minr; r <= maxr; r++)
		GD_rank(model)[r].v = N_NEW(ranksize[r]+1,Agnode_t*);	/* NIL at end */

	/* set up mincross running parameters */
	GD_pass(model) = 0;
	GD_lastwin(model) = 0;
	GD_mintry(model) = gvgetint(ug,"minpass",24);
	GD_maxpass(model) = gvgetint(ug,"maxpass",1024);
	GD_bestcrossings(model) = MAXINT;
	return model;
}

static Agraph_t *universegraph(Agraph_t *ug)
{
	graph_t		*root = ug->root;
	graph_t		*univ,*model;
	if ((model = GD_model(root))) 
		univ = model->root;
	else {
		univ = agopen("_universe\001_",AGDIGRAPHSTRICT);
	}
	return univ;
}

static void clusterchains(Agraph_t *model, Agraph_t *ug)
{
	if (GD_clusterchainsdone(ug)) return;
	if (is_a_cluster(ug))
		(void) model_skel(model,ug);
	GD_clusterchainsdone(ug) = TRUE;
}

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

static boolean run(Agraph_t *mg)
{
	if (GD_pass(mg) > GD_maxpass(mg)) return FALSE;
	if (GD_pass(mg) - GD_lastwin(mg) > GD_mintry(mg)) return FALSE;
	GD_pass(mg) = GD_pass(mg) + 1;
	return TRUE;
}

static void mincross_clust(Agraph_t *ug)
{
	Agraph_t	*g;
	g = GD_model(ug);
	if (run(g)) {
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
static void apply_model(Agraph_t *g)
{
	Agnode_t *n;
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) 
			ND_order(n) = ND_order(ND_rep(n));
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

int transpose_onerank(graph_t* g, int r, boolean reverse)
{
	int     i,c0,c1,rv;
	node_t  *v,*w;

	rv = 0;
	GD_rank(g)[r].candidate = FALSE;
	for (i = 0; i < GD_rank(g)[r].n - 1; i++) {
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

static void transpose_sweep(graph_t* g, int reverse)
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

static void mincross_sweep(graph_t* g, int dir, boolean reverse)
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


static int left2right(graph_t *g, node_t *v, node_t *w)
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

static void build_flat_graphs(graph_t *g)
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
static boolean medians(graph_t *g, int r0, int r1)
{
	static int *list;
	static int list_extent;
	int     i,j,lm,rm,lspan,rspan;
	node_t  *n,**v;
	edge_t  *e;
	boolean hasfixed = FALSE;

	if (list_extent < GD_maxinoutdeg(g)) {
		list_extent = GD_maxinoutdeg(g);
		if (!list) list = realloc(list,sizeof(list[0])*list_extent);
		else list = realloc(list,sizeof(list[0])*list_extent);
	}
	v = GD_rank(g)[r0].v;
	for (i = 0; i < GD_rank(g)[r0].n; i++) {
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
	ep = &vlist[GD_rank(g)[r].n];
	
	for (i = 0; i < GD_rank(g)[r].n; i++) {
		lp = &vlist[0];
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

/* returns level of (model) node's cluster */
static int lev(Agnode_t *n)
{
	if (!n) return -1;
	return (GD_level(ND_cluster(n)));
}

/* 
 * allocates a vpath_t per original user edge.  later the path
 * contents will be set to define vnode chains of intercluster edges.
 */
static void interclusterpaths(Agraph_t *ug, Agraph_t *topmodel)
{
	Agnode_t	*n, *n0;
	Agedge_t	*e, *ue;
	vpath_t		*p;

	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstout(ug,n); e; e = agnxtout(ug,e)) {
			if (ND_cluster(e->tail) != ND_cluster(e->head)) {
				if (!(p = pathsearch(ug,e->tail,e->head))) {
					p = newpath(e->tail, e->head);
					pathinsert(topmodel->root,p);
				}
			}
		}
	}

	/* scan all vnodes of model graph and make decisions */
	for (n = agfstnode(topmodel); n; n = agnxtnode(topmodel,n)) {
		if (ND_type(n) == NODETYPE_VNODE) {
			ue = ND_erep(n);
			p = pathsearch(topmodel->root,ue->tail,ue->head);
			if (p) {
				if (!(n0 = p->v[ND_rank(n)]) || (lev(n0) < lev(n)))
					p->v[ND_rank(n)] = n;
			}
		}
	}
	/* need to remove the other edges of these vnodes!! */
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
			globr[ND_rank(n)].n += 1;
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

/* install nodes from rank r of (g or its clusters) into globr. */
static void installglob(Agraph_t *ug, Agraph_t *fromgraph, rank_t *globr, int r)
{
	rank_t		*myrank;
	int				i;
	Agnode_t	*v;
	Agedge_t	*uedge;
	vpath_t		*path;

	if (is_a_cluster(fromgraph)) {
		myrank = &GD_rank(fromgraph)[r];
		i = 0;
		while ((v = myrank->v[i++])) {
			switch (ND_type(v)) {
				case NODETYPE_REAL:
						/* install primitive nodes */
					globr[r].v[globr[r].n++] = v;
					ND_isglobal(v) = TRUE;
					break;
				case NODETYPE_VNODE:
					/* install vnode for non-intercluster edges, and for intercluster
						 edges if this vnode was chosen as the path representative */
					uedge = ND_erep(v);
					path = pathsearch(fromgraph->root,uedge->tail,uedge->head);
					if (!path || (path->v[r] == v)) {
						globr[r].v[globr[r].n++] = v;
						ND_isglobal(v) = TRUE;
					}
					break;
				case NODETYPE_CNODE:
					/* install clusters recursively */
					installglob(ug,ND_cluster(v),globr,r);
					break;
				case NODETYPE_XNODE:
					/* we're ignoring these */
					break;
			}
		}
	}
}

/*
	after making the global graph, delete all non-global objects.
 */
static void removejunk(Agraph_t *ug, Agraph_t *topmodel)
{
	Agnode_t	*v,*vv;

	for (v = agfstnode(topmodel); v; v = vv) {
		vv = agnxtnode(topmodel,v);
		if (!ND_isglobal(v)) agdelete(topmodel,v);
	}
}

/* 
 fix up the user edge paths in the global graph.
*/
static void reconnect(Agraph_t *ug, Agraph_t *topmodel)
{
	Agnode_t	*n, *n0, *n1;
	Agedge_t	*e, *e0;
	vpath_t		*p;
	int				i;

	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstout(ug,n); e; e = agnxtout(ug,e)) {
			if (ND_cluster(e->tail) != ND_cluster(e->head)) {
				p = pathsearch(ug,e->tail,e->head);
				n0 = p->v[p->low];
				for (i = p->low + 1; i <= p->high; i++) {
					n1 = p->v[i];
					if (!(e0 = p->e[i])) e0 = agedge(topmodel,n0,n1);
					ED_weight(e0) += ED_weight(e);
					ED_xpenalty(e0) += ED_xpenalty(e);
					n0 = n1;
				}
			}
		}
	}
}

#ifdef NOTDEF
/* (no parallel edges, all forward edges, flat cycles broken) */
void build_main_graph(Agraph_t *g)
{
	Agraph_t	*gprime,*gprimeflat;
	Agnode_t	*v,*vprime;
	Agedge_t	*eprev;

	gprime = agopen("model",AGDIGRAPHSTRICT);
	for (v = agfstnode(g); v; v = agnxtnode(g,v)) {
			vprime = agnode(gprime,v->name);
			for (e = agfstout(v); e; e = agxntout(g,e)) {
				tail = e->tail;
				head = e->head;
				if (ND_rank(tail) > ND_rank(head)) {Agnode_t *t = head; head = tail; tail = t;}
				if (!(eprev = agfindedge(gprime,tail,head))) eprev = agedge(gprime,tail,head);
				merge(eprev,e);
				associate(e,eprev);
			}
	}
	break_cycles(gprime);
}
#endif

/* I. initialization */
static void cluster_init(Agraph_t *ug)

{
	Agraph_t	*mg;
	scangraph(ug);
	mg = cluster_model(ug);
	flat_edges(mg);
	build_ranks(mg,TRUE);
}

static void rec_cluster_init(Agraph_t *ug)
{
	Agraph_t	*subg;

	if (is_a_cluster(ug)) cluster_init(ug);
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(ug,subg))
		rec_cluster_init(subg);
}
