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

#ifndef GPRSTATE_H
#define GPRSTATE_H

#include <sfio.h>
#ifdef USE_CGRAPH
#include <cgraph.h>
#else
#include <agraph.h>
#endif
#include <ast.h>
#include <vmalloc.h>

    typedef enum { TV_flat, TV_bfs, TV_dfs, TV_fwd, TV_rev, TV_ne,
	    TV_en } trav_type;

    typedef struct {
	Agraph_t *curgraph;
	Agraph_t *target;
	Agraph_t *outgraph;
	Agobj_t *curobj;
	Sfio_t *tmp;
	char *tgtname;
	char *infname;
	Sfio_t *outFile;
	trav_type tvt;
	Agnode_t *tvroot;
	int name_used;
	int argc;
	char **argv;
    } Gpr_t;

    typedef struct {
	Sfio_t *outFile;
	int argc;
	char **argv;
    } gpr_info;

    extern Gpr_t *openGPRState(void);
    extern void initGPRState(Gpr_t *, Vmalloc_t *, gpr_info *);
    extern int validTVT(int);

#endif

#ifdef __cplusplus
}
#endif
