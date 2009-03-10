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

#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"
#include "glcomptexture.h"


extern glCompSet* glCompSetNew(void);
extern void glCompSetClear(glCompSet * s);
extern int glCompSetDraw(glCompSet * s);
extern int glCompSetHide(glCompSet * s);
extern int glCompSetShow(glCompSet * s);
extern int glCompSetClick(glCompSet * s, int x, int y);
extern int glCompSetRelease(glCompSet * s, int x, int y);

extern void glCompDrawBegin();
extern void glCompDrawEnd();

/*
	change all components's fonts  in s 
	to sourcefont
*/
/* void change_fonts(glCompSet * s,const texFont_t* sourcefont); */



#endif
