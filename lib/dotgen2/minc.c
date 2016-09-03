#include "newdot.h"

/* given:
 *		ND_rank(v)	 		int			integer level assignments
 *		ND_ranksize(v)  int			number of levels (assumed >= 1)
 *  	ED_xpenalty(e) 	float 	crossing penalty factor
 * find:
 *		ND_order(v)			int			index within rank
 *		ND_pos(v)				coord		(define: is this the center point?)
 *		ED_pos(e)				coord*	(define: polyline?)
 *
 *		todo: node, edge, graph labels, flat edges
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
 *		GD_level				int				distance from layout root
 *		GD_rank(g)			rank_t[]
 *		GD_repdict(g)		Dict_t*		local representatives of user objects
 *		ND_component(v) int				connected component number
 *		ND_order(v)			int				order within rank
 *		ND_type(v)			int
 *			NODE, TALLNODE  primitive node
 *			EXTNODE	represents endpoint of an external cluster edge
 *			SKELETON represents an internal cluster
 *			PATH	for virtual nodes in edges of all kinds
 *		ND_cluster(v)		graph*		for EXTNODE, its lowest user cluster
															  for SKELETON, its user cluster
																for PATH, NODE, TALLNODE  0?
 *		ND_sortweight(v)			for mincross and presort before mincross
 *		ND_sortweight_defined(v)	(sortweight undefined if up/down degree==0)
 *		ND_isglobal(v)						marks objects for the global merged graph
 */

/* TODO
	where do we use port.defined?
 */

/* forward declarations */
static void transpose_sweep(Agraph_t* g, int reverse);
static void mincross_sweep(Agraph_t* g, int dir, boolean reverse);
static void restorebest(graph_t *g);
static Agraph_t *globalize(Agraph_t *user);
static int crossings(graph_t *g);
static void rec_cluster_init(Agraph_t *ug);
static int left2right(Agraph_t *g, node_t *v, node_t *w);
static int in_cross(node_t *v,node_t *w);
static int out_cross(node_t *v, node_t *w);
static boolean medians(Agraph_t *g, int r0, int r1);
static void reorder(graph_t *g, int r, boolean reverse, boolean hasfixed);
static void savebest(graph_t *g);
static Agnode_t *choosenode(Agnode_t *fst, Agnode_t *snd);
static void cluster_ranksetup(Agraph_t *user);
static void cluster_extents(Agraph_t *g);
static void cluster_rankstorage(Agraph_t *g);
static Agnode_t *bindnode(Agraph_t *model, Agnode_t *orignode);


/*** basic user<->model association maps ***/
/* determine canonical order of n0, n1 */
static void getlowhigh(Agnode_t **n0, Agnode_t **n1)
{
	Agnode_t	*temp;
	int				d;
	d =  ND_rank(*n1) - ND_rank(*n0);
	if ((d < 0) || ((d == 0) && (AGID(*n1)< AGID(*n0))))
		{temp = *n0; *n0 = *n1; *n1 = temp;}
}

static int inrange(int a, int b, int c)
{
	return ((a <= b) && (b <= c));
}

static int repkeycmp(Dt_t *d, void *arg0, void *arg1, Dtdisc_t *disc)
{
	void *key0 = ((repkey_t*)arg0)->key;
	void *key1 = ((repkey_t*)arg1)->key;
	int		rv;
#ifdef NOTDEF
	/* not sure why I coded this - SCN   12/27/2005 */
	switch(agobjkind(key0)) {
	case AGNODE:
		rv = AGID(*(Agnode_t*)key1) - AGID(*(Agnode_t*)key0);
		break;
	case AGEDGE:
		rv = AGID(*(Agedge_t*)key1) - AGID(*(Agedge_t*)key0);
		break;
	case AGRAPH:
		/* in libgraph we don't seem to have graph ids, so use pointer */
		rv = (unsigned long)key1 - (unsigned long) key0;
		break;
	default:
		rv = 0;
	}
#else
	rv = (int)((unsigned long)key1 - (unsigned long) key0);
#endif
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
	if (association(model,key).type == 0) abort();
}

/* given a value, find (some) rep (including the key) that maps to it. 
 * for now we'll brute force it.  note that val may always be an external
 * edge path, so we wouldn't necessarily need the full inverse map.
 */
static repkey_t invassociation(Agraph_t *model, void *val)
{
	Dict_t	*dict;
	repkey_t *p;
	dict = repdict(model);
	for (p = dtfirst(dict); p; p = dtnext(dict,p)) {
		if (p->val.p == val) break;
	}
	assert(p);
	return *p;
}

typedef struct component_s {
	int		n;
	node_t	**root;
	int r;
} component_t;



/* from level.c - eventually clean this up. */
static int is_a_cluster(Agraph_t *g)
{
	return ((g == g->root) || (!strncasecmp(agnameof(g),"cluster",7)));
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

static void *T_base(void *array, int low, int size)
{
	char 	*rv;
	rv = array;
	rv = rv + low * size;
	return rv;
}

static void freearray(void *array, int low, int size)
{
	free(T_base(array,low,size));
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
	path->low = low; path->high = high;
	path->v = nodearray(low, high);
	path->e = edgearray(low, high);
	for (i = low; i <= high; i++) {
		if ((i == low) && u) path->v[i] = u;
		else if ((i == high) && v) path->v[i] = v;
		else {
			path->v[i] = agnode(model,0,TRUE);
			ND_rank(path->v[i]) = i;
		}
		if (i > low) path->e[i-1] = agedge(model,path->v[i-1],path->v[i],0,TRUE);
	}
	return path;
}

static void attributepath(vpath_t *path, int nodetype, Agraph_t *clust)
{
	int		i;
	for (i = path->low; i <= path->high; i++) {
		ND_type(path->v[i]) = nodetype;
		ND_cluster(path->v[i]) = clust;
	}
}

static rep_t model_edge(Agraph_t *model, Agedge_t *orig)
{
	Agedge_t	*e;
	Agnode_t	*low, *high, *u, *v;
	port_t		lowport, highport;
	vpath_t		*path;
	rep_t			rep;
	int				i;

	rep = association(model,orig);
	if (rep.type == 0) {
		low = agtail(orig); high = aghead(orig);
		getlowhigh(&low,&high);
		u = bindnode(model,low);
		assert(u);
		v = bindnode(model,high);
		assert(v);
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

	/* deal with ports.  note that ends could be swapped. */
	if (ND_rank(agtail(orig)) <= ND_rank(aghead(orig))) {
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
	return rep;
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
		rep.type = NODE;
		v = rep.p = agnode(model,agnameof(orig),TRUE);
		ND_rank(v) = ND_rank(orig);
	}
	else {							/* multi-rank node case */
		rep.type = TALLNODE;
		rep.p = newpath(model,NILnode,ND_rank(orig),NILnode,ND_rank(orig)+nr-1);
		attributepath(rep.p,TALLNODE,0);
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

	/* assume endpoint is represented by one node, even if orig is multi-rank.
	 * also we aren't keeping track of ports yet */
	rep.p = v = agnode(model,agnameof(orig),TRUE);
	rep.type = EXTNODE;
	ND_rank(v) = ND_rank(orig);	/* should be ND_rank(orig)+ranksize(orig)? */
	associate(model,orig,rep);
	return rep;
}

/* bind/construct representative of an internal cluster of a model graph */
static rep_t model_clust(Agraph_t *model, Agraph_t *origclust)
{
	rep_t		rep;
	vpath_t *path;

	rep = association(model,origclust);
	if (rep.type) return rep;

	rep.p = path = newpath(model,NILnode,GD_minrank(origclust),NILnode,GD_maxrank(origclust));
	rep.type = SKELETON;
	associate(model,origclust,rep);
	return rep;
}

/* helper functions for model_edge */
static rep_t rep_of_node(Agraph_t *model, Agnode_t *orignode)
{
	rep_t	rep;
	if (agcontains(GD_usergraph(model),orignode))
		rep = model_intnode(model,orignode);
	else
		rep = model_extnode(model,orignode);
	return rep;
}

static Agnode_t *bindnode(Agraph_t *model, Agnode_t *orignode)
{
	rep_t	rep;
	rep = rep_of_node(model,orignode);
	switch (rep.type) {
	case NODE: case EXTNODE: return (Agnode_t*)(rep.p);
	default: return ((vpath_t*)(rep.p))->v[ND_rank(orignode)];
	}
}

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
		if (ND_component(aghead(e)) < 0)
			search_component(g,aghead(e),c);
	for (e = agfstin(g,n); e; e = agnxtin(g,e))
		if (ND_component(agtail(e)) < 0)
			search_component(g,agtail(e),c);
}

static int ND_comp_cmpf(const void *arg0, const void *arg1)
{
	return ND_component(*(Agnode_t**)arg1) - ND_component(*(Agnode_t**)arg0);
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
		if (ND_component(n) < 0)
			search_component(g,n,compcnt++);
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
static void build_ranks(Agraph_t *ug, boolean down)
{
	queue			*q;
	component_t c;
	int				r;
	Agnode_t	*n;
	Agedge_t	*e;
	Agraph_t	*g;

	g = GD_model(ug);
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
							if (--ND_priority(aghead(e)) == 0) enqueue(q,aghead(e));
					}
					else {
						for (e = agfstin(g,n); e; e = agnxtin(g,e))
							if (--ND_priority(agtail(e)) == 0) enqueue(q,aghead(e));
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

static void set_presortkey(Agnode_t *n)
{
	int			key;
	vpath_t	*skel;
	Agraph_t *model, *parmodel;
	Agnode_t	*orignode, *parnode;

	model = agraphof(n);
	parmodel = GD_model(GD_parent(GD_usergraph(model)));
	skel = association(parmodel,GD_usergraph(model)).p;
	switch(ND_type(n)) {
	case EXTNODE:
		orignode = association(model,n).p;
		parnode = association(parmodel,orignode).p;
		if ((skel->low <= ND_rank(n)) && (ND_rank(n) <= skel->high)) {
			int		r = ND_rank(n);
			if (ND_order(parnode) < ND_order(skel->v[r]))
				key = ND_order(GD_rank(model)[r].v[leftmost(model,ND_rank(n))]) -
					(ND_order(skel->v[ND_rank(n)]) - ND_order(parnode));
			break;
		}
		else {
			key = ND_order(parnode);
		}
		break;
	default:
			key = ND_order(n);
			break;
	}
	ND_sortweight(n) = key;
	ND_sortweight_defined(n) = TRUE;
}

/* helper function */
static int presort_cmpf(const void *arg0, const void *arg1)
{
	return ND_sortweight(*(Agnode_t**)arg1) - ND_sortweight(*(Agnode_t**)arg0);
}

/*
 * sort the nodes of a subgraph so EXTNODEs are adjusted
 * according to the parent cluster before the main mincross
 * phase runs.
 */
static void presort(Agraph_t *ug)
{
	int		r;
	int		i;
	Agraph_t	*mg;

	if (ug == ug->root) return;
	mg = GD_model(ug);
	for (r = GD_minrank(mg); r <= GD_maxrank(mg); r++) {
		for (i = leftmost(mg,r); i < rightmost(mg,r); i++)
			set_presortkey(GD_rank(mg)[r].v[i]);
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
	repkey_t	*ent;
	Dict_t		*d;
	double		frac;

	/* walk all the paths */
	model = GD_model(ug);
	d = repdict(model);
	for (ent = dtfirst(d); ent; ent = dtnext(d,ent)) {
		if (ent->val.type != PATH) continue;
		p = ent->val.p;
		if ((ND_type(p->v[p->low])) == SKELETON) {
			x = p->v[p->low];
			clustmodel = GD_model(ND_cluster(x));
			frac = (ND_order(x) + 1) / GD_rank(clustmodel)[ND_rank(x)].n;
			e = p->e[p->low];
			ED_tailport(e).p.x = 2 * PORTRANGE * (frac - 0.5) + p->avgtailport;
		}
		if ((ND_type(p->v[p->high])) == SKELETON) {
			x = p->v[p->high];
			clustmodel = GD_model(ND_cluster(x));
			frac = (ND_order(x) + 1) / GD_rank(clustmodel)[ND_rank(x)].n;
			e = p->e[p->high-1];
			ED_headport(e).p.x = 2 * PORTRANGE * (frac - 0.5) + p->avgheadport;
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
	Agraph_t	*glob;

	glob = globalize(root);
	fprintf(stderr,"%s: %d crossings\n",agnameof(root),crossings(glob));
}

/* this assumes one level per node - no mega-nodes */
static void apply_model(Agraph_t *ug)
{
	Agnode_t *un;
	rep_t			rep;
	for (un = agfstnode(ug); un; un = agnxtnode(ug,un))  {
		rep = association(GD_globalgraph(ug),un);
		switch (rep.type) {
		case NODE:
			ND_order(un) = ND_order((Agnode_t*)(rep.p));
			break;
		case TALLNODE:
			ND_order(un) = ND_order(((vpath_t*)rep.p)->v[0]);
			break;
		default:
			abort();
			break;
		}
	}
}

/* this is a first cut at a top-level planner.  it's lame. */
static void rec_cluster_run(Agraph_t *ug)
{
	Agraph_t	*subg;

	if (is_a_cluster(ug)) mincross_clust(ug);
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(subg))
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

  for (e2 = agfstout(agraphof(w),w); e2; e2 = agnxtout(agraphof(w),e2)) {
		register int cnt = ED_xpenalty(e2);
		inv = ND_order(aghead(e2));
		for (e1 = agfstout(agraphof(v),v); e1; e1 = agnxtout(agraphof(v),e1)) {
			t = ND_order(aghead(e1)) - inv;
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
                                                                                
  for (e2 = agfstin(agraphof(w),w); e2; e2 = agnxtin(agraphof(w),e2)) {
		register int cnt = ED_xpenalty(e2);
		inv = ND_order(agtail(e2));
		for (e1 = agfstin(agraphof(v),v); e1; e1 = agnxtin(agraphof(v),e1)) {
			t = ND_order(agtail(e1)) - inv;
			if ((t > 0) || ((t == 0) && (ED_tailport(e1).p.x > ED_tailport(e2).p.x)))
				cross += ED_xpenalty(e1) * cnt;
    }
	}
	return cross;
}

static int int_cmpf(const void *arg0, const void *arg1)
{
	return *(int*)arg1 - *(int*)arg0;
}

/* 8 is the number of bits in port.order, an unsigned char */
#define VAL(node,port) ((ND_order(node) << 8)  + (port).order)

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
			{if (ED_xpenalty(e) > 0) list[j++] = VAL(aghead(e),ED_headport(e));}
		else for (e = agfstin(g,n); e; e = agnxtin(g,e))
			{if (ED_xpenalty(e) > 0) list[j++] = VAL(agtail(e),ED_tailport(e));}
		switch(j) {
			case 0:
				ND_sortweight(n) = MAXINT;		/* no neighbor - median undefined */
				ND_sortweight_defined(n) = FALSE;
				break;
			case 1:
				ND_sortweight(n) = list[0];
				ND_sortweight_defined(n) = TRUE;
				break;
			case 2:
				ND_sortweight(n) = (list[0] + list[1])/2;
				ND_sortweight_defined(n) = TRUE;
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
				ND_sortweight_defined(n) = TRUE;
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
		while ((lp < ep) && (!ND_sortweight_defined(*lp))) lp++;
		if (lp >= ep) break;
		/* find the node that can be compared */
		muststay = FALSE;
		for (rp = lp + 1; rp < ep; rp++) {
			if (left2right(g,*lp,*rp)) { muststay = TRUE; break; }
			if (ND_sortweight_defined(*rp)) break;	/* weight defined; it's comparable */
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
	return ND_order(*(Agnode_t**)arg1) - ND_order(*(Agnode_t**)arg0);
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

/* support function for globalize.  merge two external edge vnode vectors.
 * generally, we will prefer the vnode from the lowest (closest to a leaf)
 * cluster, but there can be ties so we work from both ends of the vector
 * to get symmetry.
 */
static void mergevec(Agnode_t **v0, Agnode_t **v1, int low, int high)
{
	int 	i,j;

	i = low;
	j = high;
	while (i <= j) {
		v0[i] = choosenode(v0[i],v1[i]);
		v0[j] = choosenode(v1[j],v0[j]);
		i++;
		j--;
	}
}

/* see above.  fst has to win ties. */
static Agnode_t *choosenode(Agnode_t *fst, Agnode_t *snd)
{
	if (GD_level(agraphof(fst)) <= GD_level(agraphof(snd))) return fst;
	return snd;
}

/* get the vnodes of an edge to an EXTNODE but where the
 * edge 'leaves' the cluster, ascend to its parent.
 * I am not sure this is really necessary or if it would
 * suffice to just take the vpath of the edge.
 */
static Agnode_t  **peel(Agraph_t *model, vpath_t *path)
{
	Agnode_t	**rv, *v;
	Agraph_t	*pmodel;
	int		r;
	repkey_t	repkey;
	Agedge_t	*origedge;
	vpath_t		*parpath;

	rv = nodearray(path->low, path->high);
	for (r = path->low; r <= path->high; r++) {
		v = path->v[r];
		if (inrange(GD_minrank(model),r,GD_maxrank(model))) rv[r] = v;
		else {
			pmodel = GD_parent(model);
			while (!inrange(GD_minrank(pmodel),r,GD_maxrank(model)) )
				model = GD_parent(model);
			/* we seem to be assuming 1-1 relationship between origedges and vpath */
			repkey = invassociation(model,path);
			origedge = repkey.key;
			parpath = association(pmodel,origedge).p;
			v = parpath->v[r];
			rv[r] = v;
		}
	}
	return rv;
}

/* helper function for globalize */
static void installglobalobjects(Agraph_t *src, Agraph_t *dst, int r)
{
	int				i;
	Agnode_t	*v, *vx;
	Agraph_t	*clustmodel;

	for (i = leftmost(src,r); i <= rightmost(src,r); i++) {
		v = GD_rank(src)[r].v[i];
		if ((vx = ND_globalobj(v)))
			install(dst,vx);
		else {
			if (ND_type(v) == SKELETON) {
				clustmodel = GD_model(ND_cluster(v));
				installglobalobjects(clustmodel,dst,r);
			}
			else {} /* ignore various external edges */
		}
	}
}

/* build the global (flat) graph of the universe.  this is 'flat'
in the sense that there is one data structure for the entire graph
(not 'flat' in the sense of flat edges within the same level.)
*/
static Agraph_t *globalize(Agraph_t *user)
{
	Agraph_t	*glob;
	Agnode_t	*v;
	vpath_t		*path, *globpath;
	int				i, r;
	Agnode_t	*n;
	Agedge_t	*e;
	rep_t			rep;
	rep_t			globrep, trep, hrep;
	Agraph_t	*model, *tmodel, *hmodel;
	Agnode_t	**tvec, **hvec;

	glob = agopen("globalgraph",Agstrictdirected,0);
	GD_usergraph(glob) = user;

	/* mark the global objects from various clusters */
	for (n = agfstnode(user); n; n = agnxtnode(user,n)) {
		globrep = model_intnode(glob, n);
		model = GD_model(ND_cluster(n));
		rep = association(model,n);
		switch(rep.type) {
		case NODE:
			ND_globalobj(v = rep.p) = globrep.p;
			break;
		case TALLNODE:
			path = rep.p;
			globpath = globrep.p;
			for (i = path->low; i < path->high; i++)
				ND_globalobj(path->v[i]) = globpath->v[i];
			break;
		default: abort();
		}
	}
	for (n = agfstnode(user); n; n = agnxtnode(user,n)) {
		for (e = agfstout(user,n); e; e = agnxtout(user,e)) {
			globrep = model_edge(glob,e);
			tmodel = GD_model(ND_cluster(agtail(e)));
			hmodel = GD_model(ND_cluster(aghead(e)));
			if (tmodel == hmodel) {		/* easy case */
				rep = association(tmodel,e);
				assert(rep.type == PATH);
				path = rep.p;
				globpath = globrep.p;
				for (i = path->low; i < path->high; i++)
					ND_globalobj(path->v[i]) = globpath->v[i];
			}
			else {
				trep = association(tmodel,e);
				tvec = peel(tmodel,trep.p);
				hrep = association(hmodel,e);
				hvec = peel(hmodel,hrep.p);
				path = (vpath_t*)(trep.p);
				mergevec(tvec,hvec,path->low,path->high);
				globpath = globrep.p;
				for (i = path->low; i <= path->high; i++)
					ND_globalobj(tvec[i]) = globpath->v[i];
				freearray(tvec,path->low,sizeof(tvec[0]));
				freearray(hvec,path->low,sizeof(hvec[0]));
			}
		}
	}

	cluster_extents(glob);
	cluster_rankstorage(glob);

	/* install new representative objects */
	for (r = GD_minrank(glob); r <= GD_maxrank(glob); r++)
		installglobalobjects(GD_model(user),glob,r);
	GD_globalgraph(user) = glob;
	return glob;
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
				r0 = ND_rank(agtail(e));
				r1 = ND_rank(aghead(e));
				low = MIN(r0,r1);
				high = MAX(r0,r1);
				for (i = low + 1; i < high; i++)
					globr[i].n += 1;
			}
	}
}

/* --------- */

static void rec_model_subclusts(Agraph_t *model, Agraph_t *user)
{
	Agraph_t	*subg;

	if (is_a_cluster(user))
		(void) model_clust(model,user);
	/* note there can be non-cluster subgraphs that contain lower clusters */
	for (subg = agfstsubg(user); subg; subg = agnxtsubg(subg))
		rec_model_subclusts(model,subg);
}

static void cluster_contents(Agraph_t *ug)
{
	Agraph_t	*mg, *root, *subg;
	char *name;
	Agnode_t	*n;
	Agedge_t	*e;

	assert(is_a_cluster(ug));
	assert(GD_model(ug) == 0);

	name = malloc(strlen(agnameof(ug))+10);
	sprintf(name,"model_%s",agnameof(ug));
	GD_model(ug) = mg = agopen(name,Agstrictdirected,0);
	free(name);
	GD_usergraph(mg) = ug;
	GD_level(mg) = GD_level(ug);

	/* install internal nodes */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		if (ND_cluster(n) == ug)
			model_intnode(mg,n);
	}

	/* install cluster skeletons */
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(subg))
		rec_model_subclusts(mg,subg);

	/* install external edge endpoints */
	root = ug->root;
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstout(root,n); e; e = agnxtout(root,e)) {
			if (!agcontains(ug,agtail(e)))
				model_extnode(mg,agtail(e));
			if (!agcontains(ug,aghead(e)))
				model_extnode(mg,aghead(e));
		}
	}

	/* install edges */
	for (n = agfstnode(ug); n; n = agnxtnode(ug,n)) {
		for (e = agfstout(root,n); e; e = agnxtout(root,e)) {
			model_edge(mg,e);		/* amazing if this is all it takes */
		}
		/* also need to scan in-edges not seen above */
		for (e = agfstin(root,n); e; e = agnxtin(root,e)) {
			if (!agcontains(ug,agtail(e)))
				model_edge(mg,e);
		}
	}
}

static void cluster_init(Agraph_t *ug)
{
	cluster_contents(ug);
	cluster_ranksetup(ug);
	flat_edges(ug);
	build_ranks(ug,TRUE);
}

static void rec_cluster_init(Agraph_t *ug)
{
	Agraph_t	*subg;

	if (is_a_cluster(ug)) cluster_init(ug);
	for (subg = agfstsubg(ug); subg; subg = agnxtsubg(subg))
		rec_cluster_init(subg);
}

#ifdef NOTDEF
/* a given edge can have several other edges (forward or backward)
   between the same endpoints.  here, we choose one of these to be
	 the canonical representative of those edges. */
static Agedge_t* canonical_edge(Agedge_t* e)
{
	Agraph_t	*g;
	Agedge_t	*canon;

	g = agraphof(e);
	if (ND_rank(aghead(e)) > ND_rank(agtail(e)))
		canon = agfindedge(g,agtail(e),aghead(e));
	else {
		if 
	}
}
#endif

static void cluster_extents(Agraph_t *g)
{
	Agnode_t	*n;
	Agedge_t	*e;
	short			indeg, outdeg;

	/* find minrank, maxrank, in/out/max node degrees */
	n = agfstnode(g);
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

/* utility function to allocate GD_rank and GD_rank(g)[r].v */
static void cluster_rankstorage(Agraph_t *g)		/* g is a model graph */
{
	Agnode_t	*n;
	int				minr,maxr,r,*ranksize;

	/* initialize storage for ranks in the model; nodes are installed later */
	minr = GD_minrank(g);
	maxr = GD_maxrank(g);
	ranksize = intarray(minr,maxr);
	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		ranksize[ND_rank(n)]++;
	GD_rank(g) = T_array(minr,maxr,sizeof(rank_t));
	for (r = minr; r <= maxr; r++)
		GD_rank(g)[r].v = N_NEW(ranksize[r]+1,Agnode_t*);	/* NIL at end */
	freearray(ranksize,minr,sizeof(ranksize[0]));
}

/* set GD_minrank, GD_maxrank, ND_indeg, ND_outdeg, GD_maxinoutdeg
 * allocate GD_rank and GD_rank(g)[r].v
 */
static void cluster_ranksetup(Agraph_t *ug)
{
	Agraph_t	*g;

	g = GD_model(ug);
	if (agfstnode(g) == NILnode) return;

	cluster_extents(g);
	cluster_rankstorage(g);

	/* set up mincross running parameters */
	GD_pass(g) = 0;
	GD_lastwin(g) = 0;
	GD_mintry(g) = gvgetint(ug,"minpass",24);
	GD_maxpass(g) = gvgetint(ug,"maxpass",1024);
	GD_bestcrossings(g) = MAXINT;
}

static int printfn(Dict_t *dict, void *arg, void *data)
{
	repkey_t *rk = arg;
	printf("%lx=>%d,%lx\n",(uint64_t)(rk->key),rk->val.type,(uint64_t)(rk->val.p));
	return 0;
}
static void dumpdict(Agraph_t *model)
{
	dtwalk(repdict(model),printfn, 0);
}
