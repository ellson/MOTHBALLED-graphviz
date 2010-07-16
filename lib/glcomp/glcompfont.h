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
#ifndef glCompFont_H
#define glCompFont_H

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
#if 0
    void copy_font(glCompFont * targetfont, const glCompFont * sourcefont);
    fontset_t *fontset_init(void);
    void free_font_set(fontset_t * fontset);
    glCompFont *add_font(fontset_t * fontset, char *fontdesc, int fs);
    glCompFont *add_glut_font(fontset_t * fontset, void *glutfont);
    void fontColor(glCompFont * font, float r, float g, float b, float a);
    glCompFont *font_init(void);
    int glCompLoadFontPNG(char *name, int id);
    void glprintf(glCompFont *, GLfloat, GLfloat, GLfloat, GLfloat, char *);
#endif

    void glprintfglut(void *font, GLfloat xpos, GLfloat ypos, GLfloat zpos, char *bf);

    glCompFont *glNewFont(glCompSet * s, char *text, glCompColor * c,
          glCompFontType type, char *fontdesc, int fs,int is2D);
    glCompFont *glNewFontFromParent(glCompObj * o, char *text);
    void glDeleteFont(glCompFont * f);
    void glCompDrawText(glCompFont * f,GLfloat x,GLfloat y);
    void glCompRenderText(glCompFont * f, glCompObj * parentObj);
    void glCompDrawText3D(glCompFont * f,GLfloat x,GLfloat y,GLfloat z,GLfloat w,GLfloat h);

#ifdef __cplusplus
}
#endif
#endif
