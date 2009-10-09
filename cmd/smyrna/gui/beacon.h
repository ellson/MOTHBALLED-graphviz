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

#ifndef BEACON_H
#define BEACON_H
#include "smyrnadefs.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern int pick_node(topview_node * n);
    extern int is_node_picked(topview_node * n);
    extern int remove_from_pick_list(topview_node * n);
    extern int add_to_pick_list(topview_node * n);
    extern int draw_node_hint_boxes(void);
    extern void pick_node_from_coords(float x, float y, float z);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
