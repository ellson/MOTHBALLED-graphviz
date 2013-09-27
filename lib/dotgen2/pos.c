void dot_position(Agraph_t *model)
{
  make_variables();
	node_separation_constraints();
	cluster_containment_constraints();
	edge_cost_constraints();
}

/* g is a model graph */
static void node_separation_constraints(Agraph_t *g)
{
	rank_t	*r;


	for (i = GD_minrank(g); i <= GD_maxrank(g); i++) {
		r = GD_rank(g)[i];
		left = NILnode;
		for (j = leftmost(g,r); j <= rightmost(g,r); j++) {
			right = r->v[j];
			node_sep(left,right);
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
	Xg = constraint_graph(u_outer);
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

static void edge_cost_constraints()
{
}
