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

#ifndef BLOCKTREE_H
#define BLOCKTREE_H

#include <render.h>
#include <circular.h>

    extern block_t *createBlocktree(Agraph_t * g, circ_state * state);
    extern void freeBlocktree(block_t *);
#ifdef DEBUG
    extern void print_blocktree(block_t * sn, int depth);
#endif

#endif

#ifdef __cplusplus
}
#endif
