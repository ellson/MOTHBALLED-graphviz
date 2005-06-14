/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/



#ifndef _GRAPH_H
#define _GRAPH_H 1

#if _PACKAGE_ast
#include    <ast.h>
#else
#include <sys/types.h>
#include <stdlib.h>
#endif
#include <stdio.h>
#include "cdt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAIL_ID				"tailport"
#define HEAD_ID				"headport"

    typedef struct Agraph_t Agraph_t;
    typedef struct Agnode_t Agnode_t;
    typedef struct Agedge_t Agedge_t;
    typedef struct Agdict_t Agdict_t;
    typedef struct Agsym_t Agsym_t;
    typedef struct Agdata_t Agdata_t;
    typedef struct Agproto_t Agproto_t;

    typedef char *(*gets_f) (char *ubuf, int n, FILE * fp);

#define AGFLAG_DIRECTED		(1<<0)
#define AGFLAG_STRICT		(1<<1)
#define AGFLAG_METAGRAPH	(1<<2)

#define	AGRAPH				0
#define	AGRAPHSTRICT		(AGRAPH | AGFLAG_STRICT)
#define AGDIGRAPH 			AGFLAG_DIRECTED
#define AGDIGRAPHSTRICT		(AGDIGRAPH | AGFLAG_STRICT)
#define AGMETAGRAPH			(AGFLAG_DIRECTED | AGFLAG_STRICT | AGFLAG_METAGRAPH)

#define AG_IS_DIRECTED(g)	((g)->kind & AGFLAG_DIRECTED)
#define AG_IS_STRICT(g)		((g)->kind & AGFLAG_STRICT)
#define AG_IS_METAGRAPH(g)	((g)->kind & AGFLAG_METAGRAPH)
#define aginit()			aginitlib(sizeof(Agraph_t),sizeof(Agnode_t),sizeof(Agedge_t))

    struct Agraph_t {
	int tag:4;
	int kind:4;
	int handle:24;
	char **attr;
	char *name;
	Agdata_t *univ;
	Dict_t *nodes, *inedges, *outedges;
	Agraph_t *root;
	Agnode_t *meta_node;
	Agproto_t *proto;
	Agraphinfo_t u;
    };

    struct Agnode_t {
	int tag:4;
	int pad:4;
	int handle:24;
	char **attr;
	char *name;
	int id;
	Agraph_t *graph;
	Agnodeinfo_t u;
    };

    struct Agedge_t {
	int tag:4;
	int printkey:4;
	int handle:24;
	char **attr;
	Agnode_t *head, *tail;
	int id;
	Agedgeinfo_t u;
    };

    struct Agdata_t {		/* for main graph */
	Dict_t *node_dict;
	Agdict_t *nodeattr;
	Agdict_t *edgeattr;
	Agdict_t *globattr;
	int max_node_id, max_edge_id;
    };

    struct Agsym_t {
	char *name, *value;
	int index;
	unsigned char printed;
	unsigned char fixed;
    };

    struct Agdict_t {
	char *name;
	Dict_t *dict;
	Agsym_t **list;
    };

    struct Agproto_t {
	Agnode_t *n;
	Agedge_t *e;
	Agproto_t *prev;
    };

#if _PACKAGE_ast
     _BEGIN_EXTERNS_		/* public data */
#if _BLD_graph && defined(__EXPORT__)
#define extern  __EXPORT__
#endif
#if !_BLD_graph && defined(__IMPORT__) && 0
#define extern  __IMPORT__
#endif
#endif
    extern char *agstrcanon(char *, char *);
    extern char *agcanonical(char *);
    extern int aghtmlstr(char *s);
    extern char *agget(void *, char *);
    extern char *agxget(void *, int);
    extern int agset(void *, char *, char *);
    extern int agxset(void *, int, char *);
    extern int agindex(void *, char *);

    extern void aginitlib(int, int, int);
    extern Agraph_t *agopen(char *, int);
    extern Agraph_t *agsubg(Agraph_t *, char *);
    extern Agraph_t *agfindsubg(Agraph_t *, char *);
    extern void agclose(Agraph_t *);
    extern Agraph_t *agread(FILE *);
    extern Agraph_t *agread_usergets(FILE *, gets_f);
    extern void agreadline(int);
    extern void agsetfile(char *);
    extern Agraph_t *agmemread(char *);
    extern int agwrite(Agraph_t *, FILE *);
    extern int agerrors(void);
    extern Agraph_t *agprotograph(void);
    extern Agraph_t *agusergraph(Agnode_t *);
    extern int agnnodes(Agraph_t *);
    extern int agnedges(Agraph_t *);

    extern void aginsert(Agraph_t *, void *);
    extern void agdelete(Agraph_t *, void *);
    extern int agcontains(Agraph_t *, void *);

    extern Agnode_t *agnode(Agraph_t *, char *);
    extern Agsym_t *agnodeattr(Agraph_t *, char *, char *);
    extern Agnode_t *agfindnode(Agraph_t *, char *);
    extern Agnode_t *agfstnode(Agraph_t *);
    extern Agnode_t *agnxtnode(Agraph_t *, Agnode_t *);
    extern Agnode_t *aglstnode(Agraph_t *);
    extern Agnode_t *agprvnode(Agraph_t *, Agnode_t *);

    extern Agedge_t *agedge(Agraph_t *, Agnode_t *, Agnode_t *);
    extern Agsym_t *agedgeattr(Agraph_t *, char *, char *);
    extern Agedge_t *agfindedge(Agraph_t *, Agnode_t *, Agnode_t *);
    extern Agedge_t *agfstedge(Agraph_t *, Agnode_t *);
    extern Agedge_t *agnxtedge(Agraph_t *, Agedge_t *, Agnode_t *);
    extern Agedge_t *agfstin(Agraph_t *, Agnode_t *);
    extern Agedge_t *agnxtin(Agraph_t *, Agedge_t *);
    extern Agedge_t *agfstout(Agraph_t *, Agnode_t *);
    extern Agedge_t *agnxtout(Agraph_t *, Agedge_t *);

    extern Agsym_t *agraphattr(Agraph_t *, char *, char *);
    extern Agsym_t *agnodeattr(Agraph_t *, char *, char *);
    extern Agsym_t *agedgeattr(Agraph_t *, char *, char *);
    extern Agsym_t *agfindattr(void *, char *);
    extern int      agcopyattr(void *, void *);

    typedef enum { AGWARN, AGERR, AGMAX, AGPREV } agerrlevel_t;
    extern agerrlevel_t agerrno;
    extern void agseterr(agerrlevel_t);
    extern char *aglasterr(void);
    extern int agerr(agerrlevel_t level, char *fmt, ...);
    extern void agerrorf(char *fmt, ...);
    extern void agwarningf(char *fmt, ...);

    extern char *agstrdup(char *);
    extern void agstrfree(char *);

    typedef enum { AGNODE = 1, AGEDGE, AGGRAPH } agobjkind_t;
#define agobjkind(p)		((agobjkind_t)(((Agraph_t*)(p))->tag))

#define agmetanode(g)		((g)->meta_node)

#undef extern
#if _PACKAGE_ast
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif
#endif				/* _GRAPH_H */
