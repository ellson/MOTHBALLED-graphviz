/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#ifndef ATT_GRAPH_H
#define ATT_GRAPH_H

#include		<cdt.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
#ifndef NOT
#define	NOT(x)			(!(x))
#endif
#ifndef NIL
#define NIL(type)		((type)0)
#endif
#define NILgraph		NIL(Agraph_t*)
#define NILnode			NIL(Agnode_t*)
#define NILedge			NIL(Agedge_t*)
#define NILsym			NIL(Agsym_t*)

/* forward struct type declarations */
    typedef struct Agtag_s Agtag_t;
    typedef struct Agobj_s Agobj_t;	/* generic object header */
    typedef struct Agraph_s Agraph_t;	/* graph, subgraph (or hyperedge) */
    typedef struct Agnode_s Agnode_t;	/* node (atom) */
    typedef struct Agedge_s Agedge_t;	/* node pair */
    typedef struct Agdesc_s Agdesc_t;	/* graph descriptor */
    typedef struct Agmemdisc_s Agmemdisc_t;	/* memory allocator */
    typedef struct Agiddisc_s Agiddisc_t;	/* object ID allocator */
    typedef struct Agiodisc_s Agiodisc_t;	/* IO services */
    typedef struct Agdisc_s Agdisc_t;	/* union of client discipline methods */
    typedef struct Agdstate_s Agdstate_t;	/* client state (closures) */
    typedef struct Agsym_s Agsym_t;	/* string attribute descriptors */
    typedef struct Agattr_s Agattr_t;	/* string attribute container */
    typedef struct Agcbdisc_s Agcbdisc_t;	/* client event callbacks */
    typedef struct Agcbstack_s Agcbstack_t;	/* enclosing state for cbdisc */
    typedef struct Agclos_s Agclos_t;	/* common fields for graph/subgs */
    typedef struct Agrec_s Agrec_t;	/* generic runtime record */
    typedef struct Agdatadict_s Agdatadict_t;	/* set of dictionaries per graph */
    typedef struct Agedgepair_s Agedgepair_t;

/* Header of a user record.  These records are attached by client programs
dynamically at runtime.  A unique string ID must be given to each record
attached to the same object.  Libgraph has functions to create, search for,
and delete these records.   The records are maintained in a circular list,
with obj->data pointing somewhere in the list.  The search function has
an option to lock this pointer on a given record.  The application must
be written so only one such lock is outstanding at a time. */

    struct Agrec_s {
	char *name;
	Agrec_t *next;
	/* following this would be any programmer-defined data */
    };

/* Object tag for graphs, nodes, and edges.  While there may be several structs
for a given node or edges, there is only one unique ID (per main graph).  */
    struct Agtag_s {
	unsigned objtype:2;	/* see literal tags below */
	unsigned mtflock:1;	/* move-to-front lock, see above */
	unsigned attrwf:1;	/* non-default attrs written */
	unsigned seq:(sizeof(unsigned) * 8 - 6);	/* sequence no. */
	unsigned long id;	/* client  ID */
    };

    /* object tags */
#define AGRAPH				0	/* can't exceed 2 bits. see Agtag_t. */
#define AGNODE				1
#define AGOUTEDGE			2
#define AGINEDGE			3	/* (1 << 1) indicates an edge tag.   */
#define AGEDGE 				AGOUTEDGE	/* synonym in object kind args */

    /* a generic graph/node/edge header */
    struct Agobj_s {		/* 7 words *//* seq_link must be first! */
	Dtlink_t seq_link;	/* link for ordering by local sequence */
	Dtlink_t id_link;	/* link for ordering by global id */
	Agtag_t tag;
	Agrec_t *data;
    };

#define AGTAG(obj)		(((Agobj_t*)(obj))->tag)
#define AGTYPE(obj)		(AGTAG(obj).objtype)
#define AGID(obj)		(AGTAG(obj).id)
#define AGSEQ(obj)		(AGTAG(obj).seq)
#define AGATTRWF(obj)	(AGTAG(obj).attrwf)
#define AGDATA(obj)		(((Agobj_t*)(obj))->data)

/* This is the node struct allocated per graph (or subgraph).  It resides
in the n_dict of the graph.  The node set is maintained by libdict, but
transparently to libgraph callers.  Every node may be given an optional
string name at its time of creation, or it is permissible to pass NIL(char*)
for the name. */

    struct Agnode_s {		/* 12 words */
	Agobj_t base;
	Agraph_t *g;
	Dtlink_t *outid, *inid;	/* in- and out-edge trees by index */
	Agedge_t *out, *in;	/* in- and out-edge trees by seq */
    };

    struct Agedge_s {		/* 8 words, but allocated in pairs -> 16 words */
	Agobj_t base;
	Agnode_t *node;		/* the endpoint node */
    };

    struct Agedgepair_s {
	Agedge_t out, in;
    };

    struct Agdesc_s {		/* graph descriptor */
	unsigned directed:1;	/* if edges are assymmetric */
	unsigned strict:1;	/* if and self, multi-edges forbidden */
	unsigned flatlock:1;	/* if sets are flattened into lists */
	unsigned maingraph:1;	/* if this is the top level graph */
	unsigned has_cmpnd:1;	/* if may contain collapsed nodes */
	unsigned no_write:1;	/* if a temporary subgraph */
	unsigned has_attrs:1;	/* if string attr tables should be initialized */
    };

/* disciplines for external resources needed by libgraph */

    struct Agmemdisc_s {	/* memory allocator */
	void *(*open) (void);	/* independent of other resources */
	void *(*alloc) (void *state, size_t req);
	void *(*resize) (void *state, void *ptr, size_t old, size_t req);
	void (*free) (void *state, void *ptr);
	void (*close) (void *state);
    };

    struct Agiddisc_s {		/* object ID allocator */
	void *(*open) (Agraph_t * g);	/* associated with a graph */
	long (*map) (void *state, int objtype, char *str,
		     unsigned long *id, int createflag);
	long (*alloc) (void *state, int objtype, unsigned long id);
	void (*free) (void *state, int objtype, unsigned long id);
	char *(*print) (void *state, int objtype, unsigned long id);
	void (*close) (void *state);
    };

    struct Agiodisc_s {
	int (*afread) (void *chan, char *buf, int bufsize);
	int (*putstr) (void *chan, char *str);
	int (*flush) (void *chan);	/* sync */
	/* error messages? */
    };

    struct Agdisc_s {		/* user's discipline */
	Agmemdisc_t *mem;
	Agiddisc_t *id;
	Agiodisc_t *io;
    };

    /* default resource disciplines */
#if !defined(_BLD_agraph) && defined(GVDLL)
#define extern	__declspec(dllimport)
#endif

    extern Agmemdisc_t AgMemDisc;
    extern Agiddisc_t AgIdDisc;
    extern Agiodisc_t AgIoDisc;

    extern Agdisc_t AgDefaultDisc;
#undef extern

    struct Agdstate_s {
	void *mem;
	void *id;
	/* IO must be initialized and finalized outside Libgraph,
	 * and channels (FILES) are passed as void* arguments. */
    };

    typedef void (*agobjfn_t) (Agobj_t * obj, void *arg);
    typedef void (*agobjupdfn_t) (Agobj_t * obj, void *arg, Agsym_t * sym);

    struct Agcbdisc_s {
	struct {
	    agobjfn_t ins;
	    agobjupdfn_t mod;
	    agobjfn_t del;
	} graph, node, edge;
    };

    struct Agcbstack_s {	/* object event callbacks */
	Agcbdisc_t *f;		/* methods */
	void *state;		/* closure */
	Agcbstack_t *prev;	/* kept in a stack, unlike other disciplines */
    };

    struct Agclos_s {
	Agdisc_t disc;		/* resource discipline functions */
	Agdstate_t state;	/* resource closures */
	Dict_t *strdict;	/* shared string dict */
	unsigned long seq[3];	/* local object sequence number counter */
	Agcbstack_t *cb;	/* user and system callback function stacks */
	unsigned char callbacks_enabled;	/* issue user callbacks or hold them? */
	Dict_t *lookup_by_name[3];
	Dict_t *lookup_by_id[3];
    };

    struct Agraph_s {		/* 14 words */
	Agobj_t base;
	Agdesc_t desc;
	Dict_t *n_seq;		/* the node set in sequence */
	Dict_t *n_id;		/* the node set indexed by ID */
	Dict_t *e_seq;		/* template for edge set operations */
	Dict_t *e_id;		/* template for edge set operations */
	Dict_t *g_dict;		/* subgraphs - descendants */
	Agraph_t *parent, *root;	/* subgraphs - ancestors */
	Agclos_t *clos;		/* shared resources */
    };


#if defined(_PACKAGE_ast)
/* fine control of object callbacks */
#   if defined(_BLD_agraph) && defined(__EXPORT__)
#	define extern  __EXPORT__
#   endif
#   if !defined(_BLD_agraph) && defined(__IMPORT__)
#	define extern  __IMPORT__
#   endif
#endif

    extern void agpushdisc(Agraph_t * g, Agcbdisc_t * disc, void *state);
    extern int agpopdisc(Agraph_t * g, Agcbdisc_t * disc);
    extern int agcallbacks(Agraph_t * g, int flag);	/* return prev value */

/* graphs */
    extern Agraph_t *agopen(char *name, Agdesc_t desc, Agdisc_t * disc);
    extern int agclose(Agraph_t * g);
    extern Agraph_t *agread(void *chan, Agdisc_t * disc);
    extern void agreadline(int);
    extern void agsetfile(char *);
    extern Agraph_t *agconcat(Agraph_t * g, void *chan, Agdisc_t * disc);
    extern int agwrite(Agraph_t * g, void *chan);
    extern void agflatten(Agraph_t * g, int flag);
    extern int agisflattened(Agraph_t * g);
    extern int agisdirected(Agraph_t * g);
    extern int agisundirected(Agraph_t * g);
    extern int agisstrict(Agraph_t * g);

/* nodes */
    extern Agnode_t *agnode(Agraph_t * g, char *name, int createflag);
    extern Agnode_t *agidnode(Agraph_t * g, unsigned long id,
			      int createflag);
    extern Agnode_t *agsubnode(Agraph_t * g, Agnode_t * n, int createflag);
    extern Agnode_t *agfstnode(Agraph_t * g);
    extern Agnode_t *agnxtnode(Agnode_t * n);

/* edges */
    extern Agedge_t *agedge(Agnode_t * t, Agnode_t * h, char *name,
			    int createflag);
    extern Agedge_t *agidedge(Agnode_t * t, Agnode_t * h, unsigned long id,
			      int createflag);
    extern Agedge_t *agsubedge(Agraph_t * g, Agedge_t * e, int createflag);
    extern Agedge_t *agfstin(Agnode_t * n);
    extern Agedge_t *agnxtin(Agedge_t * e);
    extern Agedge_t *agfstout(Agnode_t * n);
    extern Agedge_t *agnxtout(Agedge_t * e);
    extern Agedge_t *agfstedge(Agnode_t * n);
    extern Agedge_t *agnxtedge(Agedge_t * e, Agnode_t * n);

/* generic */
    extern Agraph_t *agraphof(void *);
    extern char *agnameof(void *);
    extern int agrelabel(void *obj, char *name);	/* scary */
    extern int agrelabel_node(Agnode_t * n, char *newname);
    extern int agdelete(Agraph_t * g, void *obj);
    extern long agdelsubg(Agraph_t * g, Agraph_t * sub);	/* could be agclose */
    extern int agdelnode(Agnode_t * arg_n);
    extern int agdeledge(Agedge_t * arg_e);
    extern int agisarootobj(void *);
    extern Agobj_t *agrebind(Agraph_t * g, Agobj_t * obj);

/* strings */
    extern char *agstrdup(Agraph_t *, char *);
    extern char *agstrdup_html(Agraph_t *, char *);
    extern int aghtmlstr(char *);
    extern char *agstrbind(Agraph_t * g, char *);
    extern int agstrfree(Agraph_t *, char *);
    extern char *agcanonstr(char *, char *);
    extern char *agcanonStr(char*);

/* definitions for dynamic string attributes */
    struct Agattr_s {		/* dynamic string attributes */
	Agrec_t h;		/* common data header */
	Dict_t *dict;		/* shared dict to interpret attr field */
	char **str;		/* the attribute string values */
    };

    struct Agsym_s {		/* symbol in one of the above dictionaries */
	Dtlink_t link;
	char *name;		/* attribute's name */
	char *defval;		/* its default value for initialization */
	int id;			/* its index in attr[] */
	int kind;		/* referent object type */
    };

    struct Agdatadict_s {	/* set of dictionaries per graph */
	Agrec_t h;		/* installed in list of graph recs */
	struct {
	    Dict_t *n, *e, *g;
	} dict;
    };

    extern Agsym_t *agattr(Agraph_t * g, int kind, char *name,
			   char *value);
    extern Agsym_t *agattrsym(void *obj, char *name);
    extern Agsym_t *agnxtattr(Agraph_t * g, int kind, Agsym_t * attr);
    extern void *agbindrec(void *obj, char *name, unsigned int size,
			   int move_to_front);
    extern Agrec_t *aggetrec(void *obj, char *name, int move_to_front);
    extern int agdelrec(void *obj, char *name);
    extern void aginit(Agraph_t * g, int kind, char *rec_name,
		       int rec_size, int move_to_front);
    extern void agclean(Agraph_t * g, int kind, char *rec_name);

    extern char *agget(void *obj, char *name);
    extern char *agxget(void *obj, Agsym_t * sym);
    extern int agset(void *obj, char *name, char *value);
    extern int agxset(void *obj, Agsym_t * sym, char *value);

/* defintions for subgraphs */
    extern Agraph_t *agsubg(Agraph_t * g, char *name, int cflag);	/* constructor */
    extern Agraph_t *agidsubg(Agraph_t * g, unsigned long id, int cflag);	/* constructor */
    extern Agraph_t *agfstsubg(Agraph_t * g), *agnxtsubg(Agraph_t * subg);
    extern Agraph_t *agparent(Agraph_t * g), *agroot(Agraph_t * g);

/* set cardinality */
    extern int agnnodes(Agraph_t * g), agnedges(Agraph_t * g);
    extern int agdegree(Agnode_t * n, int in, int out);

/* memory */
    extern void *agalloc(Agraph_t * g, size_t size);
    extern void *agrealloc(Agraph_t * g, void *ptr, size_t oldsize,
			   size_t size);
    extern void agfree(Agraph_t * g, void *ptr);
    extern struct _vmalloc_s *agheap(Agraph_t * g);

/* an engineering compromise is a joy forever */
    extern void aginternalmapclearlocalnames(Agraph_t * g);

#define agnew(g,t)		((t*)agalloc(g,sizeof(t)))
#define agnnew(g,n,t)	((t*)agalloc(g,(n)*sizeof(t)))

/* error handling */
    extern void agerror(int code, char *str);
#define AGERROR_SYNTAX	1	/* error encountered in lexing or parsing */
#define AGERROR_MEMORY	2	/* out of memory */
#define AGERROR_UNIMPL	3	/* unimplemented feature */
#define AGERROR_MTFLOCK	4	/* move to front lock has been set */
#define AGERROR_CMPND	5	/* conflict in restore_endpoint() */
#define AGERROR_BADOBJ	6	/* passed an illegal pointer */
#define AGERROR_IDOVFL	7	/* object ID overflow */
#define AGERROR_FLAT 	8	/* attempt to break a flat lock */
#define AGERROR_WRONGGRAPH 9	/* mismatched graph and object */

/* abbreviations, convenience */

#define AGFIRSTNODE(g)	((Agnode_t*)dtfirst((g)->n_seq))
#define AGPREVNODE(n)	((Agnode_t*)((n)->base.seq_link.hl._left))
#define AGNEXTNODE(n)	((Agnode_t*)((n)->base.seq_link.right))
#define AGFSTOUT(n)		((Agedge_t*)((n)->out))
#define AGFSTIN(n)		((Agedge_t*)((n)->in))
#define AGPREV(e)		((Agedge_t*)((e)->base.seq_link.hl._left))
#define AGNXTE(e)		((Agedge_t*)((e)->base.seq_link.right))
/* this assumes that e[0] is out and e[1] is inedge, see edgepair in edge.c  */
#define AGIN2OUT(e)		((e)-1)
#define AGOUT2IN(e)		((e)+1)
#define AGOPP(e)		((AGTYPE(e)==AGINEDGE)?AGIN2OUT(e):AGOUT2IN(e))
#define AGMKOUT(e)		(AGTYPE(e) == AGOUTEDGE? (e): AGIN2OUT(e))
#define AGMKIN(e)		(AGTYPE(e) == AGINEDGE?  (e): AGOUT2IN(e))
#define AGTAIL(e)		(AGMKIN(e)->node)
#define AGHEAD(e)		(AGMKOUT(e)->node)
#define agtail(e)		AGTAIL(e)
#define aghead(e)		AGHEAD(e)
#define agopp(e)		AGOPP(e)

#if defined(_PACKAGE_ast)
#   if !defined(_BLD_agraph) && defined(__IMPORT__)
#	define extern  __IMPORT__
#   endif
#endif
#if !defined(_BLD_agraph) && defined(GVDLL)
#define extern	__declspec(dllimport)
#endif

    extern Agdesc_t Agdirected, Agstrictdirected, Agundirected,
	Agstrictundirected;

#undef extern
#if defined(_PACKAGE_ast)
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif
#endif
