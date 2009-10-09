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
#ifndef GLCOMPBUTTON_H
#define GLCOMPBUTTON_H

#include "glcompdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern glCompButton *glCompButtonNew(glCompObj * par, GLfloat x,
					 GLfloat y, GLfloat w, GLfloat h,
					 char *caption);
    extern int glCompSetRemoveButton(glCompSet * s, glCompButton * p);
    extern void glCompButtonDraw(glCompButton * p);
    extern void glCompButtonSetText(glCompButton * p, char *str);
    extern int glCompButtonAddPngGlyph(glCompButton * b, char *fileName);
    extern void glCompButtonClick(glCompObj * o, GLfloat x, GLfloat y,
				  glMouseButtonType t);
    extern void glCompButtonDoubleClick(glCompObj * o, GLfloat x,
					GLfloat y, glMouseButtonType t);
    extern void glCompButtonMouseDown(glCompObj * o, GLfloat x, GLfloat y,
				      glMouseButtonType t);
    extern void glCompButtonMouseIn(glCompObj * o, GLfloat x, GLfloat y);
    extern void glCompButtonMouseOut(glCompObj * o, GLfloat x, GLfloat y);
    extern void glCompButtonMouseOver(glCompObj * o, GLfloat x, GLfloat y);
    extern void glCompButtonMouseUp(glCompObj * o, GLfloat x, GLfloat y,
				    glMouseButtonType t);
    extern void glCompButtonHide(glCompButton * p);
    extern void glCompButtonShow(glCompButton * p);

#ifdef __cplusplus
}
#endif
#endif
