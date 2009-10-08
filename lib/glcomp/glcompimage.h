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
#ifndef GLCOMPIMAGE_H
#define GLCOMPIMAGE_H

#include "glcompdefs.h"

extern glCompImage *glCompImageNew(glCompObj* par,GLfloat x, GLfloat y);
extern void glCompImageDelete (glCompImage* p);
extern int glCompImageLoad(glCompImage* i,unsigned char* data,int width,int height);
extern int glCompImageLoadPng(glCompImage* i,char* pngFile);
extern void glCompImageDraw(void* obj);
extern void glCompImageClick(glCompObj * o,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompImageDoubleClick(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompImageMouseDown(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);
extern void glCompImageMouseIn(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompImageMouseOut(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompImageMouseOver(glCompObj * obj,GLfloat x,GLfloat y);
extern void glCompImageMouseUp(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t);






#endif
