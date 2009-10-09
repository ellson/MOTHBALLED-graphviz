/* $Id$Revision: */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/
#ifndef GLCOMPMOUSE_H
#define GLCOMPMOUSE_H

#include "glcompdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*events*/
    extern void glCompMouseInit(glCompMouse * m);
    extern void glCompClick(glCompObj * o, GLfloat x, GLfloat y,
			    glMouseButtonType t);
    extern void glCompDoubleClick(glCompObj * obj, GLfloat x, GLfloat y,
				  glMouseButtonType t);
    extern void glCompMouseDown(glCompObj * obj, GLfloat x, GLfloat y,
				glMouseButtonType t);
    extern void glCompMouseIn(glCompObj * obj, GLfloat x, GLfloat y);
    extern void glCompMouseOut(glCompObj * obj, GLfloat x, GLfloat y);
    extern void glCompMouseOver(glCompObj * obj, GLfloat x, GLfloat y);
    extern void glCompMouseUp(glCompObj * obj, GLfloat x, GLfloat y,
			      glMouseButtonType t);
    extern void glCompMouseDrag(glCompObj * obj, GLfloat dx, GLfloat dy,
				glMouseButtonType t);

#ifdef __cplusplus
}
#endif
#endif
