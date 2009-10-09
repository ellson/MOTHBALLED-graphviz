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

/*Open GL basic component set
  includes glPanel,glCompButton,glCompCustomButton,clCompLabel,glCompStyle
*/
#ifndef GLCOMPSET_H
#define GLCOMPSET_H

#include "glcompfont.h"
#include "glcomptextpng.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern void glCompInitCommon(glCompObj * childObj,
				 glCompObj * parentObj, GLfloat x,
				 GLfloat y);
    void glCompEmptyCommon(glCompCommon * c);
    extern glCompSet *glCompSetNew(int w, int h);
    extern void glCompSetClear(glCompSet * s);
    extern int glCompSetDraw(glCompSet * s);
    extern int glCompSetHide(glCompSet * s);
    extern int glCompSetShow(glCompSet * s);
    extern int glCompSetClick(glCompSet * s, int x, int y);
    extern int glCompSetRelease(glCompSet * s, int x, int y);
    extern void glcompsetUpdateBorder(glCompSet * s, int w, int h);
    extern int glcompsetNextGroupId(glCompSet * s);
    extern int glcompsetGetGroupId(glCompSet * s);
    extern void glCompDrawBegin(void);
    extern void glCompDrawEnd(void);
    extern void glCompDeleteTexture(glCompTex * t);
    extern void glCompSetAddObj(glCompSet * s, glCompObj * obj);
    glCompObj *glCompGetObjByMouse(glCompSet * s, glCompMouse * m,
				   int onlyClickable);
    extern void glCompGetObjectType(glCompObj * p);
/*
	change all components's fonts  in s 
	to sourcefont
*/
/* void change_fonts(glCompSet * s,const texFont_t* sourcefont); */

#ifdef __cplusplus
}
#endif
#endif
