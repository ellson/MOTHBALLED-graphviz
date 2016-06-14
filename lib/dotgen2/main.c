#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "newdot.h"

void readin_attrs(graph_t *g)
{
	node_t	*n;
	edge_t	*e;
	Agsym_t	*ap_weight, *ap_minlen;

	ap_weight = agattr(g,AGEDGE,"weight","1");
	ap_minlen = agattr(g,AGEDGE,"minlen","1");
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		for (e = agfstout(g,n); e; e = agnxtout(g,e)) {
			ED_weight(e) =atoi(agxget(e,ap_weight));
			ED_minlen(e) = atoi(agxget(e,ap_minlen));
		}
	}
}

void attach_attributes(graph_t *g)
{
	node_t	*n;
	Agsym_t	*rank, *order;
	char	buf[64];

	rank = agattr(g,AGNODE,"rank","");
	order = agattr(g,AGNODE,"order","");
	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		sprintf(buf,"%d",ND_rank(n));
		agxset(n,rank,buf);
		sprintf(buf,"%d",ND_order(n));
		agxset(n,order,buf);
	}
}

void init_graph(graph_t *g)
{
	aginit(g,AGRAPH,"graphviz",sizeof(Agraphinfo_t),TRUE);
	aginit(g,AGNODE,"graphviz",sizeof(Agnodeinfo_t),TRUE);
	aginit(g,AGEDGE,"graphviz",sizeof(Agedgeinfo_t),TRUE);
}

int main(int argc, char *argv[])
{
	graph_t	*g;
	FILE		*in;
	if (argc > 1) in = fopen(argv[1],"r");
	else in = stdin;
	g = agread(in,0);
	init_graph(g);
	readin_attrs(g);
	//dot_levels(g);
//	if (argc <= 2) dot_mincross(g);
	attach_attributes(g);
	agwrite(g,stdout);
	return 1;
}
