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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMPILE_H
#define COMPILE_H

#include <sfio.h>
#include <parse.h>
#include <gprstate.h>
#include <expr.h>

    typedef struct {
	Exnode_t *guard;
	Exnode_t *action;
    } case_stmt;

#define UDATA "userval"

    typedef struct {
	Agrec_t h;
	Extype_t xu;
	Extype_t iu;
    } nval_t;

    typedef struct {
	Agrec_t h;
	Extype_t xu;
	Extype_t iu;
	char lock;
    } gval_t;

    typedef struct {
	Agrec_t h;
	Extype_t xu;
    } uval_t;

#define OBJ(p) ((Agobj_t*)p)

    typedef nval_t ndata;
    typedef uval_t edata;
    typedef gval_t gdata;

#define nData(n)    ((ndata*)(aggetrec(n,UDATA,0)))
#define gData(g)    ((gdata*)(aggetrec(g,UDATA,0)))

#define SRCOUT    0x1
#define INDUCE    0x2

    typedef struct {
	Expr_t *prog;
	Exnode_t *begin_stmt;
	Exnode_t *begg_stmt;
	int n_nstmts;
	int n_estmts;
	case_stmt *node_stmts;
	case_stmt *edge_stmts;
	Exnode_t *endg_stmt;
	Exnode_t *end_stmt;
    } comp_prog;

    extern comp_prog *compileProg(parse_prog *, Gpr_t *, int);
    extern int usesGraph(comp_prog *);
    extern int walksGraph(comp_prog *);
    extern Agraph_t *readG(Sfio_t * fp);
    extern Agraph_t *openG(char *name, Agdesc_t);
    extern Agraph_t *openSubg(Agraph_t * g, char *name);
    extern Agnode_t *openNode(Agraph_t * g, char *name);
    extern Agedge_t *openEdge(Agnode_t * t, Agnode_t * h, char *key);

#endif

#ifdef __cplusplus
}
#endif
