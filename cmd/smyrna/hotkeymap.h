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

#ifndef HOTKEYMAP_H
#define HOTKEYMAP_H
#include "smyrnadefs.h"

#define B_LSHIFT	    65505
#define B_RSHIFT	    65506
#define B_LCTRL		    65507
#define B_RCTRL		    65508
#define LEFT_MOUSE_BUTTON	glMouseLeftButton
#define RIGHT_MOUSE_BUTTON	glMouseRightButton
#define MIDDLE_MOUSE_BUTTON	glMouseMiddleButton



/*key combinations defined here,dont modify the code above here*/
#define F_BUTTON1   B_LSHIFT
#define F_BUTTON2   B_RSHIFT
#define F_BUTTON3   B_LCTRL
#define F_BUTTON4   B_LCTRL

#define MOUSE_BUTTON_1	LEFT_MOUSE_BUTTON
#define MOUSE_BUTTON_2  RIGHT_MOUSE_BUTTON
#define MOUSE_BUTTON_3	MIDDLE_MOUSE_BUTTON



extern void load_mouse_actions (char* modefile,ViewInfo* v);

extern int get_mode(ViewInfo* v);
extern int get_key_action(ViewInfo* v,int key);




#endif
