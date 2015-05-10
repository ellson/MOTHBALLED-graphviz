#include <stdio.h>
#include "cgraph.h"

int main(int argc, char **argv)
{
    Agraph_t *g;
    Agnode_t *u, *v;
	int rv;

	g = agread(stdin,0);
	if (argc >= 3) {
		u = agnode(g,argv[1],FALSE);
		v = agnode(g,argv[2],FALSE);
		rv = agnodebefore(u,v);
		fprintf(stderr,"agnodebefore returns %d\n",rv);
		fprintf(stderr,"dtsize %d\n",dtsize(g->n_seq));
	}
	agwrite(g,stdout);
}
