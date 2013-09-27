#include "newdot.h"

static Agraph_t *constraint_graph(Agraph_t *model);
static void node_separation_constraints(Agraph_t *user);
static void cluster_containment_constraints(Agraph_t *upar, Agraph_t *ug);
static void edge_cost_constraints(Agraph_t *user);

void dot_position(Agraph_t *user)
{
	Agraph_t	*cg;
	cg = constraint_graph(user);
	node_separation_constraints(user);
	cluster_containment_constraints(user);
	edge_cost_constraints(user);
	rank(cg);
	readout(user);
	agclose(cg);
}

static Agraph_t *constraint_graph(Agraph_t *model)
{
	if (!GD_model(model->root))
		GD_model(model->root) = agopen("Xconstraint",AGDIGRAPHSTRICT);
	return GD_model(model->root);
}

static void node_sep(Agraph_t *g, Agnode_t *left, Agnode_t *right)
{
	Agraph_t	*Xg;
	Agnode_t_	*leftvar; *rightvar;
	double	d0, d1;

	d0 = ND_xsize(left) / 2.0;
	d1 = ND_xsize(right) / 2.0;
	/* what about self edge sizes */
	Xg = constraint_graph(g);
	leftvar = nodevar(left);
	rightvar = nodevar(right);
	e = agedge(Xg, leftvar, rightvar);
	ED_weight(e) = 0;
	ED_minlen(e) = XSCALE(d0 + d1);
}

/* g is a model graph */
static void node_separation_constraints(Agraph_t *user)
{
	Agraph_t	*g;
	rank_t	*r;

	g = GD_model(user);
	for (i = GD_minrank(g); i <= GD_maxrank(g); i++) {
		r = GD_rank(g)[i];
		left = NILnode;
		for (j = leftmost(g,r); j <= rightmost(g,r); j++) {
			right = r->v[j];
			node_sep(g,left,right);
			right = left;
		}
	}
}

/*
 * this function has to traverse the subgraph hierarchy from the
 * user graph because we flattened out the model subgraphs.
 */
static void cluster_containment_constraints(Agraph_t *upar, Agraph_t *ug)
{
	Agraph_t	*container;

	if (is_a_cluster(upar) && is_a_cluster(ug))
		cluster_contain(upar,ug);
	container = is_a_cluster(ug)? ug : upar;
	for (subg = agfstsub(ug); subg; subg = agnxtsubg(ug,subg))
		cluster_containment_constraints(container,subg);
}

static void cluster_contain(Agraph_t *u_outer, Agraph_t *u_inner)
{
	Agraph_t	*Xg;
	int		sep;
	Agnode_t	*outer_left, *outer_right, *inner_left, *inner_right;

	sep = user_cluster_sep(u_inner);
	Xg = constraint_graph(GD_model(u_outer));
	outer_left = leftvar(GD_model(u_outer));
	outer_right = rightvar(GD_model(u_outer));
	inner_left = leftvar(GD_model(u_inner));
	inner_right = rightvar(GD_model(u_inner));
	clust_sep_edge(outer_left,inner_left);
	clust_sep_edge(inner_right,outer_right);
}

static void clust_sep_edge(Agraph_t *Xg, Agnode_t *left, Agnode_t *right, int sep)
{
	Agedge_t	*e;

	e = agedge(Xg, left, right);
	ED_weight(e) = 0;
	ED_minlen(e) = sep;
}

static int vedge_cost(int r, int low, int high)
{
	if (low + 1 == high) return SHORT_FACTOR;
	if ((r == low) || (r == high)) return LONG_END_FACTOR;
	return LONG_FACTOR;
}

static void edge_cost_constraints(Agraph_t *user)
{
	Agraph_t *model;
	Agraph_t *Xg;		/* constraints */
	Dict_t	*dict;	/* set of paths in the model graph */
	vpath_t	*p;

	model = GD_model(user);
	Xg = constraint_graph(model);
	dict = GD_pathdict(model);
	for (p = dtfirst(dict); p; p = dtnext(dict,p)) {
		v0 = nodevar(p->v[p->low]);
		for (i = p->low; i < p->high; i++) {
			v1 = nodevar(p->v[i+1]);
			vx = agnode(Xg,0);
			e0 = agedge(Xg,vx,v0);
			e1 = agedge(Xg,vx,v1);
			ED_weight(e0) = ED_weight(e1) = p->weight * vedge_factor(i,low,high);

			tp = (i == p->low?) p->tailport : 0;
			hp = (i == p->high - 1?) p->headport : 0;
		  m0 = hp - tp;
			if (m0 > 0) ED_minlen(e0) = m0;
			else ED_minlen(e1) = -m0; 
		}
	}
}
