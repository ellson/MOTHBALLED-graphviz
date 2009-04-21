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
#include "glcomppanel.h"
#include "glcomptexture.h"
#include "glcomptext.h"


glCompPanel *glCompPanelNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,glCompOrientation orientation)
{
    glCompPanel *p;
    p = malloc(sizeof(glCompPanel));
    p->color.R = GLCOMPSET_PANEL_COLOR_R;
    p->color.G = GLCOMPSET_PANEL_COLOR_G;
    p->color.B = GLCOMPSET_PANEL_COLOR_B;
    p->color.A = GLCOMPSET_PANEL_COLOR_ALPHA;
    p->shadowcolor.R = GLCOMPSET_PANEL_SHADOW_COLOR_R;
    p->shadowcolor.G = GLCOMPSET_PANEL_SHADOW_COLOR_G;
    p->shadowcolor.B = GLCOMPSET_PANEL_SHADOW_COLOR_B;
    p->shadowcolor.A = GLCOMPSET_PANEL_SHADOW_COLOR_A;
    p->shadowwidth = GLCOMPSET_PANEL_SHADOW_WIDTH;
    p->bevel = GLCOMPSET_PANEL_BEVEL;
    p->pos.x = x;
    p->pos.y = y;
    p->width = w;
    p->height = h;
	p->orientation=orientation;
	p->text=(char*)0;
    p->font = font_init();
    return p;
}
void glCompSetPanelText(glCompPanel * p,char* t)
{
	p->text=realloc(p->text,strlen(t)+sizeof(char));
	strcpy(p->text,t);
}

int glCompDrawPanel(glCompPanel * p)
{
    if (!p->visible)
	return 0;
    glColor4f(p->color.R, p->color.G, p->color.B, p->color.A);
    glBegin(GL_POLYGON);
    glVertex3f(p->pos.x, p->pos.y, p->bevel);
    glVertex3f(p->pos.x + p->width, p->pos.y, p->bevel);
    glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
    glVertex3f(p->pos.x, p->pos.y + p->height, p->bevel);
    glVertex3f(p->pos.x, p->pos.y, p->bevel);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor4f(p->shadowcolor.R, p->shadowcolor.G, p->shadowcolor.B,
	      p->color.A);
    glVertex3f(p->pos.x, p->pos.y,
	       p->bevel + (GLfloat) GLCOMPSET_BEVEL_DIFF);
    glVertex3f(p->pos.x + p->width, p->pos.y,
	       p->bevel + (GLfloat) GLCOMPSET_BEVEL_DIFF);
    glVertex3f(p->pos.x + p->width, p->pos.y + p->height,
	       p->bevel + (GLfloat) GLCOMPSET_BEVEL_DIFF);
    glVertex3f(p->pos.x, p->pos.y + p->height,
	       p->bevel + (GLfloat) GLCOMPSET_BEVEL_DIFF);
    glVertex3f(p->pos.x, p->pos.y, p->bevel);
    glEnd();
    glLineWidth(p->shadowwidth);
    glBegin(GL_LINE_STRIP);
    glColor4f((GLfloat) p->shadowcolor.R, (GLfloat) p->shadowcolor.G,
	      (GLfloat) p->shadowcolor.B, (GLfloat) p->shadowcolor.A);
    glVertex3f(p->pos.x + p->shadowwidth / ((GLfloat) 2.0),
	       p->pos.y - p->shadowwidth / ((GLfloat) 2.0), p->bevel);
    glVertex3f(p->pos.x + p->shadowwidth / (GLfloat) 2.0 + p->width,
	       p->pos.y - p->shadowwidth / (GLfloat) 2.0, p->bevel);
    glVertex3f(p->pos.x + p->shadowwidth / (GLfloat) 2.0 + p->width,
	       p->pos.y - p->shadowwidth / (GLfloat) 2.0 + p->height,
	       p->bevel);
    glEnd();
    glLineWidth(1);
	//draw text
	if(p->text)
	{


	}
    return 1;
}

int glCompSetAddPanel(glCompSet * s, glCompPanel * p)
{
    s->panelcount++;
    s->panels = realloc(s->panels, sizeof(glCompPanel *) * s->panelcount);
    s->panels[s->panelcount - 1] = p;
    p->parentset = s;
	p->font=s->fontset->fonts[s->fontset->activefont];
    return 1;
}

int glCompSetRemovePanel(glCompSet * s, glCompPanel * p)
{
    int ind = 0;
    int found = 0;
	for (; ind < s->panelcount; ind++) {
	if ((s->panels[ind] == p) && found == 0)
	    found = 1;
	if ((found == 1)&&(ind <(s->panelcount-1)))
	    s->panels[ind] = s->panels[ind + 1];
    }
    if (found) {
	free(p->text);
	free(p);
	s->panelcount--;
	s->panels =
	    realloc(s->panels, sizeof(glCompPanel *) * s->panelcount);
	return 1;
    }
    return 0;
}

int glCompPanelHide(glCompPanel * p)
{
    int ind = 0;
    for (ind = 0; ind < ((glCompSet *) p->parentset)->buttoncount; ind++) {
	if (((glCompSet *) p->parentset)->buttons[ind]->panel == p)
	    ((glCompSet *) p->parentset)->buttons[ind]->visible = 0;
    }

    for (ind = 0; ind < ((glCompSet *) p->parentset)->labelcount; ind++) {
	if (((glCompSet *) p->parentset)->labels[ind]->panel == p)
	    ((glCompSet *) p->parentset)->labels[ind]->visible = 0;
    }
    p->visible = 0;
    return 1;


}

int glCompPanelShow(glCompPanel * p)
{
    int ind = 0;
    for (ind = 0; ind < ((glCompSet *) p->parentset)->buttoncount; ind++) {
	if (((glCompSet *) p->parentset)->buttons[ind]->panel == p)
	    ((glCompSet *) p->parentset)->buttons[ind]->visible = 1;
    }

    for (ind = 0; ind < ((glCompSet *) p->parentset)->labelcount; ind++) {
	if (((glCompSet *) p->parentset)->labels[ind]->panel == p)
	    ((glCompSet *) p->parentset)->labels[ind]->visible = 1;
    }
    p->visible = 1;
    return 1;

}

