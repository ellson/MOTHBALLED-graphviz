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

#ifndef CIRCO_H
#define CIRCO_H

#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern void circo_layout(Agraph_t * g);
    extern void circoLayout(Agraph_t * g);
    extern void circo_cleanup(Agraph_t * g);
    extern void circo_init_graph(graph_t * g);

#ifdef __cplusplus
}
#endif
#endif
