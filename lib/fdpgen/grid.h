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

#ifndef GRID_H
#define GRID_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <render.h>
#include <cdt.h>

    typedef struct _grid Grid;

    typedef struct _node_list {
	Agnode_t *node;
	struct _node_list *next;
    } node_list;

    typedef struct {
	int i, j;
    } gridpt;

    typedef struct {
	gridpt p;		/* index of cell */
	node_list *nodes;	/* nodes in cell */
	Dtlink_t link;		/* cdt data */
    } cell;

    extern Grid *mkGrid(int);
    extern void adjustGrid(Grid * g, int nnodes);
    extern void clearGrid(Grid *);
    extern void addGrid(Grid *, int, int, Agnode_t *);
    extern void walkGrid(Grid *, int (*)(Dt_t *, cell *, Grid *));
    extern cell *findGrid(Grid *, int, int);
    extern void delGrid(Grid *);
    extern int gLength(cell * p);

#endif

#ifdef __cplusplus
}
#endif
