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

#ifndef NODESET_H
#define NODESET_H

#include  <render.h>

    typedef struct {
	Dtlink_t link;
	Agnode_t *np;
    } nsitem_t;

    typedef Dt_t nodeset_t;

    extern nodeset_t *mkNodeset();
    extern void freeNodeset(nodeset_t *);
    extern void clearNodeset(nodeset_t *);
    extern void insertNodeset(nodeset_t * ns, Agnode_t * n);
    extern void removeNodeset(nodeset_t *, Agnode_t * n);
    extern int sizeNodeset(nodeset_t * ns);

#ifdef DEBUG
    extern void printNodeset(nodeset_t *);
#endif

#endif

#ifdef __cplusplus
}
#endif
