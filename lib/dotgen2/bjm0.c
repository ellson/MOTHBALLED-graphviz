typedef struct Agnodeinfo_s {
	int		order;
} Agnodeinfo_t;
typedef struct Agedgeinfo_s {
	int		useless;
} Agedgeinfo_t;
typedef struct Agraphinfo_s {
	int		useless;
} Agraphinfo_t;
#include <cgraph.h>
#include <stdlib.h>
#include <radix.h>

#define ND_order(n) 		((n)->u.order)

int crossings(int r, int southsequence[], int q)
{
	int 	crosscount, firstindex, *tree, treesize;
	int	t, k, index;

	/* build the accumulator tree */
	firstindex = 1;
	while (firstindex<q) firstindex *= 2;
	treesize = 2*firstindex - 1; /* number of tree nodes */
	firstindex -= 1; /* index of leftmost leaf */
	tree = (int *) malloc(treesize*sizeof(int));
	for (t=0; t<treesize; t++) tree[t] = 0;

	/* count the crossings */
	crosscount = 0; /* number of crossings */
	for (k=0; k<r; k++) { /* insert edge k */
		index = southsequence[k] + firstindex;
		tree[index]++;
		while (index > 0) {
			if (index%2) crosscount += tree[index+1];
			index = (index - 1)/2;
			tree[index]++;
		}
	}
	printf("Number of crossings: %d\n",crosscount);
	free(tree); 
	return crosscount;
}

static int edgecmpf(void *arg0, void *arg1)
{
	int	major, minor;
	Agedge_t	*e0, *e1;

	e0 = *(Agedge_t**)arg0;
	e1 = *(Agedge_t**)arg1;
	major = ND_order(e0->tail)  - ND_order(e1->tail);
	if (major) return major;
	minor = ND_order(e0->head) - ND_order(e1->head);
	return minor;
}

int main(int argc, char **argv)
{
	Agraph_t	*g;
	Agnode_t	*n;
	Agedge_t	*e;
	radixrec_t	*edgelist;
	int		i, ne, p, q, my_order;
	int		*southseq;
	char		junk;

	aginit();
	g = agread(stdin);		/* must be two-layer */
	i = ne = p = q = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		sscanf(n->name,"%c%d",&junk,&my_order);
		ND_order(n) = my_order;
		if (agfstout(g,n)) p++;
		else q++;
	}
	southseq = malloc(agnnodes(g) * sizeof(int));
	edgelist = malloc(agnedges(g) * sizeof(radixrec_t));
	for (n = agfstnode(g); n; n = agnxtnode(g,n))
		for (e = agfstout(g,n); e; e = agnxtout(g,e), ne++) 
			{ edgelist[ne].key = (ND_order(e->tail) << 16) + ND_order(e->head);
			  edgelist[ne].data = e; }
	/*qsort(edgelist,i,sizeof(Agedge_t*),edgecmpf);*/
	radix_sort(edgelist,ne);
	for (i = 0; i < ne; i++) printf("%d %d %s %s\n",
		((edgelist[i].key & 0xffff0000) >> 16),
		((edgelist[i].key & 0x0000ffff) >> 0),
		((Agedge_t*)(edgelist[i].data))->tail->name,
		((Agedge_t*)(edgelist[i].data))->head->name);
	for (i = 0; i < ne; i++) southseq[i] = ND_order(((Agedge_t*)(edgelist[i].data))->head);
	crossings(ne, southseq, q);
}
