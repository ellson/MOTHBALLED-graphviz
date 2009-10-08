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
#ifndef GLCOMPLABEL_H
#define GLCOMPLABEL_H

#include "glcompdefs.h"


glCompLabel *glCompLabelNew(glCompObj* par,GLfloat x, GLfloat y,char *text);
extern int glCompSetAddLabel(glCompSet * s, glCompLabel * p);
extern int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p);


/*events*/
extern int glCompLabelDraw(glCompLabel * p);
extern void glCompLabelClick(glCompObj * o,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompLabelDoubleClick(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompLabelMouseDown(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompLabelMouseIn(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompLabelMouseOut(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompLabelMouseOver(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompLabelMouseUp(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);





#endif
