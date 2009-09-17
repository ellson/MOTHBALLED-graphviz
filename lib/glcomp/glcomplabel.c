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
    p->parentset = s;
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

	int kts,kts2;
	GLfloat tempX,tempY;
	GLfloat h,h2;	/*container widget height*/
	float color_fac;
    float fontx, fonty;
	GLfloat fontwidth;

	if (p->orientation==1){	kts=1; h=0;}else{kts=-1; h=((glCompSet*)p->parentset)->h;}
	if (p->panel->orientation==1){	kts2=1; h2=0;}else
	{
		kts2=-1; h2=((glCompSet*)p->panel->parentset)->h;
	}
	if ((!p->visible) || (!p->panel->visible))
		return 0;
	if (p->panel)
	{
		tempX=p->pos.x;
		tempY=p->pos.y;
		p->pos.x = p->panel->pos.x + p->pos.x;
		p->pos.y = p->panel->pos.y*kts2*kts+h2 + p->pos.y-h;
		if (p->panel->orientation==0)
			p->pos.y = p->pos.y - p->panel->height;
    }
	printf ("kts:%d h:%f kts2:%d h2:%d \n", kts , h , kts2, h2);

	p->font->fontheight=p->size;
	fontColor(p->font,p->color.R, p->color.G, p->color.B, p->color.A);

	glprintf(p->font, p->pos.x,  p->pos.y,p->panel->bevel,(p->size * p->fontsizefactor *strlen(p->text)), p->text);
	if (p->panel) {
	    p->pos.x = tempX;
	    p->pos.y = tempY;
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
