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
#ifndef GLCOMPTEXT_H
#define GLCOMPTEXT_H

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include "glpangofont.h"
typedef float GLfloat;
#else
#include <unistd.h>
#include <GL/gl.h>
#endif
#include "glcompdefs.h"
#include <GL/glut.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern fontset_t *fontset_init(void);
    extern void free_font_set(fontset_t * fontset);
//extern void glprintf (glCompText*, GLfloat , GLfloat , GLfloat ,GLfloat, char*);
//extern        void glprintfglut (void* font, GLfloat xpos, GLfloat ypos, GLfloat zpos,char *bf);


#ifdef __cplusplus
}
#endif
#endif
