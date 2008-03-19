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
#ifdef _WIN32
#include "windows.h"
#endif
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef GLCOMPTEXTURE_H
#define GLCOMPTEXTURE_H
typedef struct {
    int id;
    float w, h;
} glCompTexture;

glCompTexture *glCompCreateTextureFromRaw(char *filename, int width,
					  int height, int wrap);
int glCompDeleteTexture(glCompTexture * t);
#endif
