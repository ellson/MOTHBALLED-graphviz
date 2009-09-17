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
#include "glCompTrackBar.h"
#include "glcomptext.h"



glCompTrackBar *glCompTrackBarNew(GLfloat x, GLfloat y, GLfloat w,
				  GLfloat h, glCompOrientation orientation)
{
    glCompTrackBar *p;
    p = malloc(sizeof(glCompTrackBar));
    /*default color values for a panel */
    p->color.R = GLCOMPSET_PANEL_COLOR_R;
    p->color.G = GLCOMPSET_PANEL_COLOR_G;
    p->color.B = GLCOMPSET_PANEL_COLOR_B;
    p->color.A = GLCOMPSET_PANEL_COLOR_ALPHA;
    p->shadowcolor.R = GLCOMPSET_PANEL_SHADOW_COLOR_R;
    p->shadowcolor.G = GLCOMPSET_PANEL_SHADOW_COLOR_G;
    p->shadowcolor.B = GLCOMPSET_PANEL_SHADOW_COLOR_B;
    p->shadowcolor.A = GLCOMPSET_PANEL_SHADOW_COLOR_A;
    p->bevel = GLCOMPSET_PANEL_BEVEL;
    p->outerpanel->pos.x = x;
    p->outerpanel->pos.y = y;
    p->outerpanel->width = w;
    p->outerpanel->height = h;
    p->indicator->width = p->outerpanel->width - 2 * GLCOMPSET_DEFAULT_PAD;
    p->indicator->shadowwidth = 0;

    p->font = font_init();
    return NULL;
}
