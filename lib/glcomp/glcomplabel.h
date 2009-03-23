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


extern glCompLabel *glCompLabelNew(GLfloat x, GLfloat y, GLfloat size, char *text,glCompOrientation orientation);
extern int glCompSetAddLabel(glCompSet * s, glCompLabel * p);
extern int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p);
extern int glCompDrawLabel(glCompLabel * p);



#endif
