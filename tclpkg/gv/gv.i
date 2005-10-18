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
extern Agraph_t *stringgraph(char *graphstring);	/* from dot-formatted string */
extern Agraph_t *readgraph(char *filename);		/* from dot-formatted file */

extern Agraph_t *digraphstrict(char *name);		/* new empty digraphstrict */
extern Agraph_t *graphstrict(char *name);		/* new empty graphstrict */
extern Agraph_t *digraph(char *name);			/* new empty digraph */
extern Agraph_t *graph(char *name);			/* new empty graph */

extern Agraph_t *graph(Agraph_t *g, char *name);	/* add new empty subgraph to existing graph */
extern Agnode_t *node(Agraph_t *g, char *name);		/* add new node to existing graph */
extern Agedge_t *edge(Agnode_t *t, Agnode_t *h);	/* add new edge between existing nodes */

/* set/get attribute of graph/node/edge */
extern char *set(Agraph_t *g, char *attr, char *val=NULL);
extern char *set(Agnode_t *n, char *attr, char *val=NULL);
extern char *set(Agedge_t *e, char *attr, char *val=NULL);

/* remove graph objects */
extern void rm(Agraph_t *g);
extern void rm(Agnode_t *n);
extern void rm(Agedge_t *e);

extern void layout(Agraph_t *g, char *engine);

extern void writegraph(Agraph_t *g, char *filename, char *format);
