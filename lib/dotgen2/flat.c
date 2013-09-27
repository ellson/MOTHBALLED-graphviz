/* this function will have to be modified if we go for
 * multi-level nodes */
int is_flat_edge(Agedge_t *e)
{
	if (ND_rank(agtail(e)) = ND_rank(aghead(e)) return TRUE;
	return FALSE;
}

void model_flat_edge(Agraph_t *flat, Agedge_t *orig)
{
	Agnode_t	*u, *v;
	u = model_flat_node(flat, agtail(orig));
	v = model_flat_node(flat, aghead(orig));
	e = agedge(flat,u,v,0,TRUE);	/* could merge edges if multiple? */
}

Agraph_t *flat_init(Agraph_t *user)
{
	flat = agopen("flatgraph",Agstrictdirected,0);
	for (u = agfstnode(user); u; u = agnxtnode(user,u)) {
		for (e = agfstout(user,u); e; e = agnxtout(user,e)) {
			if (is_flat_edge(e)) 
				model_flat_edge(flat,e);
		}
	}
}

void flat_dfs(Agraph_t *flat, Agnode_t *n)
{
	ND_mark(n) = TRUE;
	ND_onstack(n) = TRUE;
	for (e = agfstout(flat,n); e; e = next_e) {
		next_e = agnxtout(g,e);
		if (ND_onstack(aghead(e))) agdelete(flat,e);
	}
	ND_onstack(n) = FALSE;
}

void flat_breakcycles(Agraph_t *flat)
{
	for (n = agfstnode(flat); n; n = agnxtnode(flat,n))
		ND_mark(n) = 0;
	for (n = agfstnode(flat); n; n = agnxtnode(flat,n))
		if (!ND_mark(n)) flat_dfs(flat,n);
}

static void reverse_edge(Agraph_t *g, edge_t *e)
{
	edge_t	*rev;

	rev = agfindedge(g,aghead(e),agtail(e));
	if (!rev) rev = agedge(g,aghead(e),agtail(e),(char*)0,TRUE);
	merge(rev,ED_minlen(e),ED_weight(e));
	agdelete(g,e);
}

void flat_phase(Agraph_t *user)
{
	flat = flat_init(user);
	flat_breakcycles(flat);
}
