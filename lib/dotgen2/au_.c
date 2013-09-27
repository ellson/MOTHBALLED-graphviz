#include "newdot.h"
#include <math.h>

int mapbool(char *str, int defval)
{
	if (str && str[0]) {
		if (!strcasecmp(str,"false")) return FALSE;
		if (!strcasecmp(str,"true")) return TRUE;
		return atoi(str);
	}
	return defval;
}

void *zmalloc(size_t nbytes)
{
    char    *rv = malloc(nbytes);
    if (nbytes == 0) return 0;
    if (rv == NULL) {fprintf(stderr, "out of memory\n"); abort();}
    memset(rv,0,nbytes);
    return rv;
}
                                                                                
void *zrealloc(void *ptr, size_t size, size_t elt, size_t osize)
{
    void    *p = realloc(ptr,size*elt);
    if (p == NULL && size) {fprintf(stderr, "out of memory\n"); abort();}
    if (osize < size) memset((char*)p+(osize*elt),'\0',(size-osize)*elt);
    return p;
}
                                                                                
int dot_Verbose;
char *CmdName = "newdot";

void warn3(char *s0, char *s1, char *s2)
{
	fprintf(stderr,"%s: ",CmdName);
	fprintf(stderr,s0,s1,s2);
}

int gvround(double arg)
{
	return (int)(arg + (arg > 0?.5:-.5));
}

int gvgetint(void *obj, char *str, int defval)
{
	char	*valstr;
	int		rv;
	double	frac;

	valstr = agget(obj,str);
	if (valstr && valstr[0]) {
		if (sscanf(valstr,"%lf%%",&frac)) rv = gvround(frac * defval);
		else if (!sscanf(valstr,"%d",&rv)) rv = defval;
	}
	else rv = defval;
	return rv;
}

Agedge_t *agfindedge(Agraph_t *g, Agnode_t *t, Agnode_t *h)
{
	return agedge(g,t,h,0,0);
}

static void my_init_node(Agraph_t *g, Agobj_t *node, void *arg)
{ int *sz = arg; agbindrec(node,"level node rec",sz[1],TRUE); }
static void my_init_edge(Agraph_t *g, Agobj_t *edge, void *arg)
{ int *sz = arg; agbindrec(edge,"level edge rec",sz[2],TRUE); }
static void my_init_graph(Agraph_t *g, Agobj_t *graph, void *arg)
{ int *sz = arg; agbindrec(graph,"level graph rec",sz[0],TRUE); }
static Agcbdisc_t mydisc = { {my_init_graph,0,0}, {my_init_node,0,0}, {my_init_edge,0,0} };

void agautoinit(Agraph_t *g, int graphinfo_size, int nodeinfo_size, int edgeinfo_size) 
{
	int *s;

	s = N_NEW(3,int);	/* until we fix something, this is a storage leak */
	s[0] = graphinfo_size; s[1] = nodeinfo_size; s[2] = edgeinfo_size;
	agpushdisc(g,&mydisc,s);
}
