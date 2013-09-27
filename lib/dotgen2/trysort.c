#include "newdot.h"

typedef struct rsort_s {
	unsigned char key[5];
	Agedge_t		*e;
} rsort_t;

main()
{
	Agraph_t	*g;
	Agnode_t	*n;
	Agedge_t	*e, **edgelist;
	g = agread(stdin);
	edgelist = alledges(g);
	radixsort(edgelist, agnedges(g), 0, endchar);
}
