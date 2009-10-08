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
#include "glcompdefs.h"


typedef struct {
    float x, y, z;
} point3f;
typedef struct {
  point3f u, v;
} line;
typedef struct {
  point3f N;   /* normal */
  double d;  /* offset */
} plane;


int GetFixedOGLPos(int, int, float, GLfloat*, GLfloat*, GLfloat * Z);
int GetOGLPosRef(int x, int y, float *X, float *Y, float *Z);
float GetOGLDistance(int l);
int GetFixedOGLPoslocal(int x, int y, GLfloat * X, GLfloat * Y, GLfloat * Z);
void to3D(int x, int y, GLfloat * X, GLfloat * Y,GLfloat * Z);
void linear_interplotate (float,float,float,float,float,float*);
double point_to_line_dist(point3f p, point3f u, point3f v);
double point_to_lineseg_dist (point3f p, point3f a, point3f b);
int rot_spherex(plane J,float tet,point3f P,point3f* P2);
void make_plane(point3f a,point3f b,point3f c,plane* P);
void replacestr(char *source,char **target);
extern void glCompCalcWidget(glCompCommon* parent,glCompCommon* child,glCompCommon* ref);
extern void glCompDrawRectangle (glCompRect* r);
extern void glCompDrawRectPrism (glCompPoint* p,GLfloat w,GLfloat h,GLfloat b,GLfloat d,glCompColor* c,int bumped);
extern void copy_glcomp_color(glCompColor* source,glCompColor* target);
extern void glCompSetColor(glCompColor* c);

void glCompSelectionBox(glCompSet* s);
#endif
