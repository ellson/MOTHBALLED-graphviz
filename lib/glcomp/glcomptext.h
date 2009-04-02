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
#define GLTEXFONTH_H

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

#ifdef __cplusplus
extern "C" {
#endif
extern fontset_t* fontset_init(void);
extern void free_font_set(fontset_t* fontset);
extern void copy_font(glCompText* targetfont,const glCompText* sourcefont);
extern int add_font(fontset_t* fontset,char* fontdesc);
extern void glprintf (glCompText*, GLfloat , GLfloat , GLfloat, char*);
extern	void glprintfglut (void* font, GLfloat xpos, GLfloat ypos, char *bf);

extern glCompText* font_init(void);
extern void fontColor (glCompText* font,float r, float g, float b,float a);
extern int glCompLoadFontPNG (char *name, int id);


#ifdef __cplusplus
}
#endif

#endif


