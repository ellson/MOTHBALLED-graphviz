#include <stdio.h>
#include "cgraph.h"
#include "bla.h"

int main(int argc, char **argv)
{
    Agraph_t *g;
    Agnode_t *n;
	Agnoderef_t *nref;
	Agedge_t *e;
	Agedgeref_t *eref;

	g = agread(stdin,0);
	for (nref = FIRSTNREF(g); nref; nref = NEXTNREF(nref)) {
		n = NODEOF(nref);
		fprintf(stderr,"%s\n",agnameof(n));
		for (eref = FIRSTOUTREF(g,nref); eref; eref = NEXTEREF(nref,eref)) {
			e = EDGEOF(nref,eref);
			fprintf(stderr,"\t %s ", agnameof(agtail(e)));
			fprintf(stderr,"-> %s\n", agnameof(aghead(e)));
		}
	}
    return 1;
}
