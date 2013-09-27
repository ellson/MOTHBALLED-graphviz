void dot_X(Agraph_t *ug)
{
	graph_t		*Xg = agopen(AGSTRICTDIRECTED,"constraints");

	make_nodevars(ug,Xg);
	make_edgevars(ug,Xg);
	make_clustvars(ug,Xg);
	constrain_nodenode(ug,Xg);
	constrain_edgelen(ug,Xg);
	constraint_nodeclust(ug,Xg);
	constraint_clustclust(ug,Xg);
	solve();
	read_nodevars(ug,Xg);
	read_clustvars(ug,Xg);
}
