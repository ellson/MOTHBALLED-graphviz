/* $Id$Revision: */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/
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
