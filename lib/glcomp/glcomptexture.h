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

/*Open GL texture handling and storing mechanism
  includes glPanel,glCompButton,glCompCustomButton,clCompLabel,glCompStyle
*/

#ifndef glCompFontURE_H
#define glCompFontURE_H

#ifdef _WIN32
#include "windows.h"
#endif
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glcompdefs.h"
extern glCompTex* glCompSetAddNewTexImage(glCompSet* s,int width,int height,unsigned char* data,int is2D);
extern glCompTex* glCompSetAddNewTexLabel(glCompSet* s, char* def,int fs,char* text,int is2D);
#endif
