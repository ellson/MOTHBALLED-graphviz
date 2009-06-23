/* $Id$Revision: */
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

#ifndef GVPR_H
#define GVPR_H

#include "cgraph.h"

  /* If set, gvpr calls exit() on errors */
#define GV_USE_EXIT 1    
  /* If set, gvpr stores output graphs in gvpropts */
#define GV_USE_OUTGRAPH 2

typedef ssize_t (*gvprwr) (void*, const char *buf, size_t nbyte, void*);

typedef struct {
    Agraph_t** ingraphs;      /* NULL-terminated array of input graphs */
    int n_outgraphs;          /* if GV_USE_OUTGRAPH set, output graphs */
    Agraph_t** outgraphs;
    gvprwr out;               /* write function for stdout */
    gvprwr err;               /* write function for stderr */
    int flags;
} gvpropts;

    extern int gvpr (int argc, char *argv[], gvpropts* opts);

#endif

#ifdef __cplusplus
}
#endif
