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

#ifndef GLPANGOFONT_H
#define GLPANGOFONT_H
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <png.h>

//creates a font file with given name and font description
//returns non-zero if fails
unsigned char* create_pango_texture(char *fontdescription,int fontsize,char* txt,cairo_surface_t *surface,int* w,int* h);
int create_font_file(char *fontdescription,int fs, char *fontfile, float gw,float gh);


#endif

