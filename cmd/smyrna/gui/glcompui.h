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

#ifndef GLCOMPUI_H
#define GLCOMPUI_H
#include "smyrnadefs.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern glCompSet *glcreate_gl_topview_menu(void);
    extern void switch2D3D(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t);
    extern void menu_click_center(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t);
    extern void menu_click_zoom_minus(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t);
    extern void menu_click_zoom_plus(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t);
    extern void menu_click_pan(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
