#include "newdot.h"

/* Simple and Efficient BiLayer Cross Counting by Barth, Mutzel and Junger */

static int count_crossings(rank_t *north, rank_t *south)
{
	int 	crosscount, firstindex;
	int		t, k, index;
	static int trip;

	trip++;
	/* build the accumulator tree */
	if (north->tree == 0) {
		firstindex = 1;
		while (firstindex < south->n) firstindex *= 2;
		north->treesize = 2*firstindex - 1; /* number of tree nodes */
		north->tree = N_NEW(north->treesize,int);
		firstindex -= 1;
	}
	else {
		firstindex = (north->treesize + 1) / 2 - 1; /* index of leftmost leaf */
	}
	for (t=0; t<north->treesize; t++) north->tree[t] = 0;

	/* count the crossings */
	crosscount = 0; /* number of crossings */
	for (k=0; k < north->ne; k++) { /* insert edge k */
		index = ND_order(aghead(((Agedge_t*)(north->edgelist[k].data)))) + firstindex;
		north->tree[index]++;
		while (index > 0) {
			if (index%2) crosscount += north->tree[index+1];
			index = (index - 1)/2;
			north->tree[index]++;
		}
	}
	return crosscount;
}

int crossings_below(Agraph_t *g, int northlevel)
{
	int		c;
	rank_t	*r;
	node_t	**v;
	edge_t	*e;

	r = &GD_rank(g)[northlevel];
	if (r->crossing_cache.valid)
		return r->crossing_cache.count;

	if (northlevel == GD_maxrank(g)) c = 0; 
	else {
		if (!r->edgelist) {	/* set up edgelist */
			/* count the edges */
			c = 0;
			for (v = r->v; *v; v++)
				for (e = agfstout(g,*v); e; e = agnxtout(g,e)) c++;
			r->edgelist = N_NEW(c+1,radixrec_t);
			/* install the edges */
			c = 0;
			for (v = r->v; *v; v++)
				for (e = agfstout(g,*v); e; e = agnxtout(g,e)) {
					r->edgelist[c].key = (ND_order(agtail(e)) << 16) + ND_order(aghead(e));
					r->edgelist[c].data = e;
					c++;
				}
			r->ne = c;
		}
		if (r->ne > 0) {
			radix_sort(r->edgelist,r->ne);
			c = count_crossings(r,&GD_rank(g)[northlevel+1]);
		}
	}
	r->crossing_cache.count = c;
	r->crossing_cache.valid = TRUE;
	return c;
}
