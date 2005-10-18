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

%module gv
%{
#define BUILTINS
#include "gvc.h"
%}

/* new graph objects */
extern Agraph_t *graph(char *name);		/* new empty graph */
extern Agraph_t *digraph(char *name);		/* new empty digraph */
extern Agraph_t *strictgraph(char *name);	/* new empty strict graph */
extern Agraph_t *strictdigraph(char *name);	/* new empty strict digraph */

extern Agraph_t *readstring(char *string);	/* from dot-formatted string */
extern Agraph_t *read(char *filename);		/* from dot-formatted file */
extern Agraph_t *read(FILE *f);			/* from dot-formatted file */

extern Agraph_t *graph(Agraph_t *g, char *name);/* add subgraph to graph */
extern Agnode_t *node(Agraph_t *g, char *name);	/* add node to graph */
extern Agedge_t *edge(Agnode_t *t, Agnode_t *h);/* add edge between existing nodes */
				
extern Agedge_t *edge(Agnode_t *t, char *hname);/* add edge, existing tail, named head which will be induced if necessary */
extern Agedge_t *edge(char *tname, Agnode_t *h);/* add edge, named tail which will be induced if necessary, existing head */

extern Agedge_t *edge(Agraph_t *g, char *tname, char *hname);/* add edge between named nodes, induced as necessary */

/* set/get attribute of graph/node/edge */
extern char *set(Agraph_t *g, char *attr, char *val=NULL);
extern char *set(Agnode_t *n, char *attr, char *val=NULL);
extern char *set(Agedge_t *e, char *attr, char *val=NULL);

extern char *set(Agraph_t *g, Agsym_t *a, char *val=NULL);
extern char *set(Agnode_t *n, Agsym_t *a, char *val=NULL);
extern char *set(Agedge_t *e, Agsym_t *a, char *val=NULL);

/* names */
extern char *nameof(Agraph_t *g);
extern char *nameof(Agnode_t *n);
//extern char *nameof(Agedge_t *e);
extern char *nameof(Agsym_t *a);

extern Agraph_t *findsubg(Agraph_t *g, char *name);
extern Agnode_t *findnode(Agraph_t *g, char *name);
extern Agedge_t *findedge(Agnode_t *t, Agnode_t *h);

extern Agsym_t *findattr(Agraph_t *g, char *name);
extern Agsym_t *findattr(Agnode_t *n, char *name);
extern Agsym_t *findattr(Agedge_t *e, char *name);

/* misc navigators */
extern Agnode_t *headof(Agedge_t *e);
extern Agnode_t *tailof(Agedge_t *e);
extern Agraph_t *graphof(Agraph_t *g);
extern Agraph_t *graphof(Agedge_t *e);
extern Agraph_t *graphof(Agnode_t *n);
extern Agraph_t *rootof(Agraph_t *g);

/* iterators */
extern bool ok(Agraph_t *g);
extern bool ok(Agnode_t *n);
extern bool ok(Agedge_t *e);
extern bool ok(Agsym_t *a);

extern Agraph_t *firstsubg(Agraph_t *g);
extern Agraph_t *nextsubg(Agraph_t *g, Agraph_t *sg);

extern Agraph_t *firstsupg(Agraph_t *g);
extern Agraph_t *nextsupg(Agraph_t *g, Agraph_t *sg);

extern Agedge_t *firstedge(Agraph_t *g);
extern Agedge_t *nextedge(Agraph_t *g, Agedge_t *e);

extern Agedge_t *firstout(Agraph_t *g);
extern Agedge_t *nextout(Agraph_t *g, Agedge_t *e);

extern Agedge_t *firstedge(Agnode_t *n);
extern Agedge_t *nextedge(Agnode_t *n, Agedge_t *e);

extern Agedge_t *firstout(Agnode_t *n);
extern Agedge_t *nextout(Agnode_t *n, Agedge_t *e);

extern Agnode_t *firsthead(Agnode_t *n);
extern Agnode_t *nexthead(Agnode_t *n, Agnode_t *h);

extern Agedge_t *firstin(Agraph_t *g);
extern Agedge_t *nextin(Agnode_t *n, Agedge_t *e);

extern Agedge_t *firstin(Agnode_t *n);
extern Agedge_t *nextin(Agraph_t *g, Agedge_t *e);

extern Agnode_t *firsttail(Agnode_t *n);
extern Agnode_t *nexttail(Agnode_t *n, Agnode_t *t);

extern Agnode_t *firstnode(Agraph_t *g);
extern Agnode_t *nextnode(Agraph_t *g, Agnode_t *n);

extern Agnode_t *firstnode(Agedge_t *e);
extern Agnode_t *nextnode(Agedge_t *e, Agnode_t *n);

extern Agsym_t *firstattr(Agraph_t *g);
extern Agsym_t *nextattr(Agraph_t *g, Agsym_t *a);

extern Agsym_t *firstattr(Agnode_t *n);
extern Agsym_t *nextattr(Agnode_t *n, Agsym_t *a);

extern Agsym_t *firstattr(Agedge_t *e);
extern Agsym_t *nextattr(Agedge_t *e, Agsym_t *a);

/* remove graph objects */
extern void rm(Agraph_t *g);
extern void rm(Agnode_t *n);
extern void rm(Agedge_t *e);

extern void layout(Agraph_t *g, char *engine);

extern void render(Agraph_t *g, char *format, char *filename);
extern void render(Agraph_t *g, char *format, FILE *f);
extern void render(Agraph_t *g, char *format, void **data);
