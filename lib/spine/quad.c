/* vim:set shiftwidth=4 ts=4: */

#include <stdlib.h>
#include <assert.h>

#include "spinehdr.h"
#include "quad.h"
#include "subset.h"

static int cmpdeg(const void *v0, const void *v1)
{
	Agnode_t *n0 = *(Agnode_t **) v0;
	Agnode_t *n1 = *(Agnode_t **) v1;

	if (ND_deg(n0) > ND_deg(n1))
		return -1;
	else if (ND_deg(n0) < ND_deg(n1))
		return 1;
	else
		return 0;
}

void genQuads(Agraph_t * g, quadfn_t action, void *state)
{
	int nnodes = agnnodes(g);
	Agnode_t **arr = N_NEW(nnodes, Agnode_t *);
	Agraph_t *cloneg = agsubg(g, "clone", 1);
	Dt_t **subs = N_NEW(nnodes, Dt_t *);
	Agnode_t *n;
	Agnode_t *v;
	Agnode_t *u;
	Agnode_t *w;
	Agedge_t *e;
	Agedge_t *f;

	/* make clone graph */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		agsubnode(cloneg, n, 1);
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			agsubedge(cloneg, e, 1);
		}
	}

	/* sort the vertices by non-increasing degrees */
	int j, i = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		arr[i++] = n;
		ND_deg(n) = agdegree(cloneg, n, 1, 1);
	}
	qsort(arr, nnodes, sizeof(Agnode_t *), cmpdeg);

	/* create index and set for each node */
	for (i = 0; i < nnodes; i++) {
		if (i < nnodes - 1)
			assert(ND_deg(arr[i]) >= ND_deg(arr[i + 1]));
		ND_id(arr[i]) = i;
		subs[i] = mkSubset();
	}

	for (i = 0; i < nnodes; i++) {
		v = arr[i];
		/* for each adjacent node u of v */
		for (e = agfstedge(cloneg, v); e; e = agnxtedge(cloneg, e, v)) {
			if (agtail(e) == aghead(e))
				continue;
			if (agtail(e) == v)
				u = aghead(e);
			else
				u = agtail(e);
			/* for each adjacent node w != v of u */
			for (f = agfstedge(cloneg, u); f; f = agnxtedge(cloneg, f, u)) {
				if (agtail(f) == aghead(f))
					continue;
				if (agtail(f) == u)
					w = aghead(f);
				else
					w = agtail(f);
				addSubset(subs[ND_id(w)], u);
			}
		}
		for (j = i + 1; j < nnodes; j++) {
			if (sizeSubset(subs[j]) >= 2)
				/* generate quadrilaterals */
				action(v, arr[j], subs[j], state);
		}
		for (j = i + 1; j < nnodes; j++) {
			if (sizeSubset(subs[j]) >= 1)
				clearSubset(subs[j]);
		}
		agdelnode(cloneg, v);
		closeSubset(subs[i]);
	}

	agclose(cloneg);
	free(arr);
	free(subs);
}

#ifdef TEST
static int walker(Agnode_t * n, int *isFirst)
{
	if (*isFirst) {
		*isFirst = 0;
		printf("%s", agnameof(n));
	} else
		printf(",%s", agnameof(n));
	return 0;
}

static void
findQuads(Agnode_t * v, Agnode_t * w, Dt_t * subset, void *state)
{
	int first = 1;
	printf("%s %s {", agnameof(v), agnameof(w));
	walkSubset(subset, (walkfn) walker, &first);
	printf("}\n");
}

int main()
{
	Agraph_t *g = agread(stdin, 0);
	genQuads(g, findQuads, 0);
	return 0;
}
#endif
