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

#ifndef DEGLIST_H
#define DEGLIST_H

/* List of nodes sorted by increasing degree */

#include  <render.h>

    typedef Dt_t deglist_t;

    extern deglist_t *mkDeglist(void);
    extern void freeDeglist(deglist_t * list);
    extern void insertDeglist(deglist_t * list, Agnode_t * n);
    extern void removeDeglist(deglist_t * list, Agnode_t * n);
    extern Agnode_t *firstDeglist(deglist_t *);

#ifdef DEBUG
    extern void printDeglist(deglist_t *);
#endif

#endif

#ifdef __cplusplus
}
#endif
