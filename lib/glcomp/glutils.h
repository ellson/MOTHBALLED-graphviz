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

#ifndef GLUTILS_H
#define GLUTILS_H
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/gl.h>
#include <GL/glu.h>


int GetFixedOGLPos(int x, int y, float kts, GLfloat * X, GLfloat * Y,
		   GLfloat * Z);
int GetOGLPosRef(int x, int y, float *X, float *Y, float *Z);
float GetOGLDistance(int l);
int GetFixedOGLPoslocal(int x, int y, float kts, GLfloat * X, GLfloat * Y,
		   GLfloat * Z);


#endif
