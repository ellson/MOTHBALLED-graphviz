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
#include "glcomplabel.h"
#include "glcomptexture.h"
#include "glcomptext.h"


glCompLabel *glCompLabelNew(GLfloat x, GLfloat y, GLfloat size, char *text,glCompOrientation orientation)
{
    glCompLabel *p;
    p = malloc(sizeof(glCompLabel));
    p->color.R = GLCOMPSET_LABEL_COLOR_R;
    p->color.G = GLCOMPSET_LABEL_COLOR_G;
    p->color.B = GLCOMPSET_LABEL_COLOR_B;
    p->color.A = GLCOMPSET_LABEL_COLOR_ALPHA;
    p->bevel = GLCOMPSET_PANEL_BEVEL + GLCOMPSET_BEVEL_DIFF;
    p->text = strdup(text);
    p->pos.x = x;
    p->pos.y = y;
    p->size = size;
    p->visible = 1;
    p->fontsizefactor = GLCOMPSET_FONT_SIZE_FACTOR;
    p->panel = '\0';
    p->font = font_init();
    return p;
}

int glCompSetAddLabel(glCompSet * s, glCompLabel * p)
{
    s->labelcount++;
    s->labels = realloc(s->labels, sizeof(glCompLabel *) * s->labelcount);
    s->labels[s->labelcount - 1] = p;
	p->font=s->fontset->fonts[s->fontset->activefont];
    return 1;
}


int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p)
{
    int ind = 0;
    int found = 0;
    for (; ind < s->labelcount - 1; ind++) {
	if ((s->labels[ind] == p) && found == 0)
	    found = 1;
	if (found == 1)
	    s->labels[ind] = s->labels[ind + 1];
    }
    if (found) {
	free(p->font);
	free(p->text);
	free(p);
	s->labelcount--;
	s->labels =
	    realloc(s->labels, sizeof(glCompLabel *) * s->labelcount);
	return 1;
    }
    return 0;
}

int glCompDrawLabel(glCompLabel * p)
{
    if (p->visible) {
	if (p->panel) {
	    p->pos.x = p->pos.x + p->panel->pos.x;
	    p->pos.y = p->pos.y + p->panel->pos.y;
	}

	p->font->fontheight=p->size;
	fontColor(p->font,p->color.R, p->color.G, p->color.B, p->color.A);

	glprintf(p->font, p->pos.x,  p->pos.y,
		        (p->size * p->fontsizefactor *
			      strlen(p->text)), p->text);
	if (p->panel) {
	    p->pos.x = p->pos.x - p->panel->pos.x;
	    p->pos.y = p->pos.y - p->panel->pos.y;
	}
	return 1;
    }
    return 0;
}
int glCompLabelSetText(glCompLabel * p, char *text)
{
    free(p->text);
    p->text = strdup(text);
    return 1;
}
