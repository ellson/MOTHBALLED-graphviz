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

#ifndef BLOCKPATH_H
#define BLOCKPATH_H

#include <circular.h>

    extern nodelist_t *layout_block(Agraph_t * g, block_t * sn, double);
    extern int cmpNodeDegree(Dt_t *, Agnode_t **, Agnode_t **, Dtdisc_t *);

#ifdef DEBUG
    extern void prTree(Agraph_t * g);
#endif

#endif

#ifdef __cplusplus
}
#endif
