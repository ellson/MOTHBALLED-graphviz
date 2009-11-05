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

#ifndef APPMOUSE_H
#define APPMOUSE_H
#include "smyrnadefs.h"

extern void appmouse_left_click_down(ViewInfo* v,int x,int y);
extern void appmouse_left_click_up(ViewInfo* v,int x,int y);
extern void appmouse_left_drag(ViewInfo* v,int x,int y);
extern void appmouse_right_click_down(ViewInfo* v,int x,int y);
extern void appmouse_right_click_up(ViewInfo* v,int x,int y);
extern void appmouse_right_drag(ViewInfo* v,int x,int y);
extern int get_mode(ViewInfo* v);

#endif
