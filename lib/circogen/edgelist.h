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

#ifndef EDGELIST_H
#define EDGELIST_H

#include  <render.h>

    typedef struct edgelistitem {
	Dtlink_t link;
	Agedge_t *edge;
    } edgelistitem;

    typedef Dt_t edgelist;

    extern edgelist *init_edgelist(void);
    extern void add_edge(edgelist * list, Agedge_t * e);
    extern void remove_edge(edgelist * list, Agedge_t * e);
    extern void free_edgelist(edgelist * list);
    extern int size_edgelist(edgelist * list);
#ifdef DEBUG
    extern void print_edge(edgelist *);
#endif

#endif

#ifdef __cplusplus
}
#endif
