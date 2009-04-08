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
#ifndef GLCOMPPANEL_H
#define GLCOMPPANEL_H

#include "glcompdefs.h"


extern glCompPanel *glCompPanelNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,glCompOrientation orientation);
extern int glCompDrawPanel(glCompPanel * p);
extern int glCompSetAddPanel(glCompSet * s, glCompPanel * p);
extern int glCompSetRemovePanel(glCompSet * s, glCompPanel * p);
extern int glCompPanelShow(glCompPanel * p);
extern int glCompPanelHide(glCompPanel * p);
extern void glCompSetPanelText(glCompPanel * p,char* t);

#endif
