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
#include "glCompTrackBar.h"
#include "glcomptext.h"
#include "memory.h"

glCompTrackBar *glCompTrackBarNew(GLfloat x, GLfloat y, GLfloat w,
				  GLfloat h, glCompOrientation orientation)
{
    glCompTrackBar *p;
    p = NEW(glCompTrackBar);
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
