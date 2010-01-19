/*********** external interfaces *********/
#ifdef NOTDEF
#include <vmalloc.h>
#endif
#include <assert.h>
#include <string.h>
//#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <values.h> */
#include <cdt.h>
#include <cgraph.h>
#include "radix.h"

/*********** a few useful types *********/
typedef unsigned char boolean;
typedef struct {double x, y;} point;
typedef struct {point LL, UR;} box;

typedef struct Agraph_s	graph_t;
typedef struct Agnode_s	node_t;
typedef struct Agedge_s	edge_t;

/*********** types needed by graphs *********/
typedef struct adjmatrix_t {	/* for flat edge constraints */
	int		nrows,ncols;
	char	*data;
} adjmatrix_t;
#define ELT(M,i,j)         (M->data[((i)*M->ncols)+(j)])

typedef struct rank_s {		 /* describes the contents of a rank */
	node_t	**v;		/* the vertex list */
	int		n;				/* its extent */

	/* for counting edge crossings */
	radixrec_t	*edgelist;	/* list of out (south) edges */
	int		ne;			/* extent of this list */
	int		*tree, treesize;	/* the crossing accumulator tree */

	adjmatrix_t	*flat;			/* transitive closure of the left-right constraints */

	struct crossing_cache_s {
		int count;						/* crossings between this level and this + 1 */
		boolean valid;
	} crossing_cache;	

	boolean	candidate;				/* true if an adjacent level changed */
	boolean	changed;
} rank_t;

typedef struct rep_s {
	void		*p;		/* must always be an Agraph_t*, Agnode_t*, Agedge_t*  */
	int			type;	/* NODE, TALLNODE, PATH, SKELETON, EXTNODE */
} rep_t;

typedef struct repkey_s {
	void		*key;
	rep_t		val;
} repkey_t;

typedef struct vpath_s {
	int				low, high;
	node_t		**v;							/* virtual nodes of path */
	edge_t		**e;							/* virtual edges of path indexed by tail rank */
	float			avgtailport, avgheadport; /* total port offsets */
	float			weight;
} vpath_t;


/*********** graphs *********/
typedef struct Agraphinfo_s {
	Agrec_t hdr;
	struct Agraph_s *parent;		/* containing cluster (not parent subgraph) */
	union {
		struct Agraph_s *user;	/* points from a model graph back to client obj */
		struct Agraph_s *global; /* points from user root graph to global model */
	} graph;
	int		level;								/* cluster nesting level (not node level!) */
	node_t	*minrep, *maxrep;	/* set leaders for min and max rank */
	boolean has_sourcerank, has_sinkrank;
	int			minrank,maxrank;

	/* these control mincross */
	struct Agraph_s	*graphrep;	/* graph that models this cluster for mincross */
	rank_t	*rank;
	int			pass;			/* a simple counter */
	int			lastwin;	/* last pass that was an improvement */
	int			mintry;		/* limit to losing passes */
	int			maxpass;	/* limit to total passes */
	int			bestcrossings;	/* current best solution */
	Dict_t	*repdict;

	/* other mincross values */
	int			maxinoutdeg;		/* maximum of all in and out degrees */
} Agraphinfo_t;


#define GD_FIELD(g,field)  (((Agraphinfo_t*)((g)->base.data))->field)
#define GD_parent(g)	GD_FIELD(g,parent)
#define GD_level(g)		GD_FIELD(g,level)
#define GD_minrep(g)	GD_FIELD(g,minrep)
#define GD_maxrep(g)	GD_FIELD(g,maxrep)
#define GD_has_sourcerank(g)	GD_FIELD(g,has_sourcerank)
#define GD_has_sinkrank(g)		GD_FIELD(g,has_sinkrank)
#define GD_model(g)		GD_FIELD(g,graphrep)
#define GD_rank(g)		GD_FIELD(g,rank)
#define GD_minrank(g)		GD_FIELD(g,minrank)
#define GD_maxrank(g)		GD_FIELD(g,maxrank)
#define GD_pass(g)			GD_FIELD(g,pass)
#define GD_lastwin(g)		GD_FIELD(g,lastwin)
#define GD_mintry(g)		GD_FIELD(g,mintry)
#define GD_maxpass(g)		GD_FIELD(g,maxpass)
#define GD_bestcrossings(g)		GD_FIELD(g,bestcrossings)
#define GD_maxinoutdeg(g)		GD_FIELD(g,maxinoutdeg)
#define GD_repdict(g)		GD_FIELD(g,repdict)
#define GD_usergraph(g)		GD_FIELD(g,graph.user)
#define GD_globalgraph(g)	 GD_FIELD(g,graph.global)

typedef struct nlist_s {
	int			size;
	node_t		**list;
} nlist_t;

typedef struct elist_s {
	int			size;
	edge_t		**list;
} elist_t;

typedef struct queue {
    node_t  **store,**limit,**head,**tail;
} queue;

/*********** nodes *********/
typedef struct Agnodeinfo_s {
	Agrec_t hdr;
	int		rank;				/* network simplex solver */
	graph_t	*cluster;			/* lowest containing cluster */
	node_t	*set;				/* for union-find */

	union {
		node_t	*v;
		edge_t	*e;
		graph_t *g;
	} rep;
	int	type;

/* tags for representative (model) objects */
/* 0 is reserved for undefined */
#define NODE						1			/* a real node on only one rank */
#define TALLNODE				2			/* a real node on two or more ranks */
#define EXTNODE					3			/* a node external to a cluster */
#define SKELETON				4
#define PATH						5

		/* for network-simplex */
	elist_t	tree_in;
	elist_t	tree_out;
	int		priority;
	int		low,lim;
	edge_t	*par;

		/* for mincross */
	short		indeg, outdeg;
	int			order;
	int			saveorder;		/* for saving the best solution */
	int			component;
	int			ranksize;			/* height as counted in number of levels */
	int			sortweight;		/* median, barycenter, etc. */
	boolean mark, onstack, sortweight_defined;
} Agnodeinfo_t;

#define ND_FIELD(n,field)  (((Agnodeinfo_t*)((n)->base.data))->field)
#define ND_globalobj(n)	ND_FIELD(n,rep.v)
#define ND_rep(n)		ND_FIELD(n,rep.v)
#define ND_cluster(n)	ND_FIELD(n,cluster)
#define ND_cluster(n)	ND_FIELD(n,cluster)
#define ND_type(n)		ND_FIELD(n,type)
#define ND_set(n)		ND_FIELD(n,set)
#define ND_mark(n) 		ND_FIELD(n,mark)
#define ND_onstack(n)	ND_FIELD(n,onstack)
#define ND_rank(n)		ND_FIELD(n,rank)
#define ND_tree_in(n)	ND_FIELD(n,tree_in)
#define ND_tree_out(n)	ND_FIELD(n,tree_out)
#define ND_priority(n)	ND_FIELD(n,priority)
#define ND_low(n)		ND_FIELD(n,low)
#define ND_lim(n)		ND_FIELD(n,lim)
#define ND_par(n)		ND_FIELD(n,par)
#define	ND_indeg(n)		ND_FIELD(n,indeg)
#define	ND_outdeg(n)	ND_FIELD(n,outdeg)
#define ND_order(n)		ND_FIELD(n,order)
#define ND_saveorder(n)		ND_FIELD(n,saveorder)
#define ND_component(n)		ND_FIELD(n,component)
#define ND_sortweight(n)	ND_FIELD(n,sortweight)
#define ND_sortweight_defined(n)	ND_FIELD(n,sortweight_defined)
#define ND_ranksize(n)		ND_FIELD(n,ranksize)

/********** edges **********/
typedef struct port_s { /* internal edge endpoint specification */
  point   p;          /* aiming point */
  double    theta;    /* slope in radians */
  box*    bp;         /* if not null, points to bbox of
											 * rectangular area that is port target
											 */
  boolean constrained,defined;
  unsigned char order;      /* for mincross (?) */
} port_t;

typedef struct Agedgeinfo_s {
	Agrec_t hdr;
	/* to assign levels */
	float		minlen;
	float		weight;
	int			tree_index;
	int			cutval;

	/* to run mincross */
	int			xpenalty;		/* crossing weight */
	port_t	tailport, headport;

	vpath_t		*path;

} Agedgeinfo_t;

#define ED_FIELD(e,field)  (((Agedgeinfo_t*)((e)->base.data))->field)
#define ED_minlen(e)	ED_FIELD(e,minlen)
#define ED_weight(e)	ED_FIELD(e,weight)
#define ED_tree_index(e) ED_FIELD(e,tree_index)
#define ED_cutval(e)	ED_FIELD(e,cutval)
#define ED_xpenalty(e) ED_FIELD(e,xpenalty)
#define ED_tailport(e) ED_FIELD(e,tailport)
#define ED_headport(e) ED_FIELD(e,headport)
#define ED_orig(e)		 ED_FIELD(e,orig)


/********** variable constants **********/
#define		BACKWARD_PENALTY	1000
#define		STRONG_CLUSTER_WEIGHT	1000
#define		VEDGE_PENALTY 	1
#define   CEDGE_PENALTY		(1<<30)-1
#define		PORTRANGE				1e+6	/* port coordinates cannot be bigger than this */

	/* xpos phase */
#define 	SHORT_FACTOR		1
#define		LONG_FACTOR			2
#define		LONG_END_FACTOR	4

/********** useful symbols **********/
#define		MINRANK		1
#define 	SOURCERANK	2
#define		MAXRANK		3
#define		SINKRANK	4
#define		SAMERANK	5
#define		NORANK		6



/********** libgraph extensions **********/
int			mapbool(char *str, int defval);

#ifndef NIL
#define NIL(t)	((t)0)	/* agrees with CDT */
#endif
#define NILgraph	NIL(Agraph_t*)
#define NILnode		NIL(Agnode_t*)
#define NILedge		NIL(Agedge_t*)


/********** useful macros **********/
#ifndef MAXINT
#define MAXINT (int)(~((int)0) ^ (1 << (sizeof(int)*8-1)))
#endif
#define MAX(a,b)	((a)>=(b)?(a):(b))
#define MIN(a,b)	((a)<=(b)?(a):(b))
#define NEW(t)      ((t*)zmalloc(sizeof(t)))
#define N_NEW(n,t)  ((t*)zmalloc((n)*sizeof(t)))
#define alloc_elist(n,L)      do {L.size = 0; L.list = N_NEW(n + 1,edge_t*); } while (0)
#define free_list(L)          do {if (L.list) free(L.list);} while (0)
#define ALLOC(size,ptr,type) (ptr? (type*)realloc(ptr,(size)*sizeof(type)):(type*)malloc((size)*sizeof(type)))


/********** graphviz entry points **********/
extern int		dot_Verbose;
void					rank(graph_t *g, int balance, int maxiter);

Agraph_t 			*dot_lca(Agraph_t *c0, Agraph_t *c1);

#include "au_.h"
