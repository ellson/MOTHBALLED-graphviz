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

#include "glcompset.h"


void glCompDrawBegin()		//pushes a gl stack 
{
    int vPort[4];

    glGetIntegerv(GL_VIEWPORT, vPort);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();


    glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//      glEnable( GL_TEXTURE_2D );

    glPushMatrix();
    glLoadIdentity();

}

void glCompDrawEnd()		//pops the gl stack 
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
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
    return 1;
}

glCompPanel *glCompPanelNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h)
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
    return p;
}

int glCompSetAddPanel(glCompSet * s, glCompPanel * p)
{
    s->panelcount++;
    s->panels = realloc(s->panels, sizeof(glCompPanel *) * s->panelcount);
    s->panels[s->panelcount - 1] = p;
    p->parentset = s;
    return 1;
}

int glCompSetAddLabel(glCompSet * s, glCompLabel * p)
{
    s->labelcount++;
    s->labels = realloc(s->labels, sizeof(glCompLabel *) * s->labelcount);
    s->labels[s->labelcount - 1] = p;
    return 1;
}

int glCompSetRemovePanel(glCompSet * s, glCompPanel * p)
{
    int ind = 0;
    int found = 0;
	for (ind; ind < s->panelcount; ind++) {
	if ((s->panels[ind] == p) && found == 0)
	    found = 1;
	if ((found == 1)&&(ind <(s->panelcount-1)))
	    s->panels[ind] = s->panels[ind + 1];
    }
    if (found) {
	free(p);
	s->panelcount--;
	s->panels =
	    realloc(s->panels, sizeof(glCompPanel *) * s->panelcount);
	return 1;
    }
    return 0;
}

int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p)
{
    int ind = 0;
    int found = 0;
    for (ind; ind < s->labelcount - 1; ind++) {
	if ((s->labels[ind] == p) && found == 0)
	    found = 1;
	if (found == 1)
	    s->labels[ind] = s->labels[ind + 1];
    }
    if (found) {
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

	fontSize((int) p->size);
	fontColorA(p->color.R, p->color.G, p->color.B, p->color.A);

	fontDrawString((int) p->pos.x, (int) p->pos.y, p->text,
		       (int) (p->size * p->fontsizefactor *
			      strlen(p->text)));
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


int glCompSetDraw(glCompSet * s)
{
    glCompDrawBegin();
    glCompSetDrawPanels(s);
    glCompDrawEnd();
    return 1;
}

int glCompSetDrawPanels(glCompSet * s)
{
    int ind = 0;
    for (ind; ind < s->panelcount; ind++) {
	glCompDrawPanel(s->panels[ind]);
    }
    for (ind = 0; ind < s->buttoncount; ind++) {
	glCompDrawButton(s->buttons[ind]);
    }
    for (ind = 0; ind < s->labelcount; ind++) {
	glCompDrawLabel(s->labels[ind]);
    }
    return 1;

}

int glCompSetHide(glCompSet * s)
{
    int ind = 0;
    for (ind; ind < s->panelcount; ind++) {
	s->panels[ind]->visible = 0;
    }
    for (ind = 0; ind < s->buttoncount; ind++) {
	s->buttons[ind]->visible = 0;
    }
    for (ind = 0; ind < s->labelcount; ind++) {
	s->labels[ind]->visible = 0;
    }
    return 1;

}

int glCompSetShow(glCompSet * s)
{
    int ind = 0;
    for (ind; ind < s->panelcount; ind++) {
	s->panels[ind]->visible = 1;
    }
    for (ind = 0; ind < s->buttoncount; ind++) {
	s->buttons[ind]->visible = 1;
    }
    for (ind = 0; ind < s->labelcount; ind++) {
	s->labels[ind]->visible = 1;
    }
    return 1;

}



glCompButton *glCompButtonNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,
			      char *caption, char *glyphfile,
			      int glyphwidth, int glyphheight)
{
    glCompButton *p;
    p = malloc(sizeof(glCompButton));
    p->color.R = GLCOMPSET_BUTTON_COLOR_R;
    p->color.G = GLCOMPSET_BUTTON_COLOR_G;
    p->color.B = GLCOMPSET_BUTTON_COLOR_B;
    p->color.A = GLCOMPSET_BUTTON_COLOR_ALPHA;
    p->bevel = GLCOMPSET_PANEL_BEVEL + GLCOMPSET_BEVEL_DIFF;
    p->thickness = GLCOMPSET_BUTTON_THICKNESS;
    p->caption = strdup(caption);
    p->pos.x = x;
    p->pos.y = y;
    p->width = w;
    p->height = h;
    p->enabled = 1;
    p->visible = 1;
    p->status = 0;		//0 not pressed 1 pressed;
    p->groupid = -1;
    p->fontsize = GLCOMPSET_FONT_SIZE;
    p->fontcolor.R = GLCOMPSET_BUTTON_FONT_COLOR_R;
    p->fontcolor.G = GLCOMPSET_BUTTON_FONT_COLOR_G;
    p->fontcolor.B = GLCOMPSET_BUTTON_FONT_COLOR_B;
    p->fontcolor.A = GLCOMPSET_BUTTON_FONT_COLOR_ALPHA;
    //load glyph
    if (glyphfile) {

	p->glyph =
	    glCompCreateTextureFromRaw(glyphfile, glyphwidth, glyphheight,
				       0);
	p->glyphwidth = glyphwidth;
	p->glyphheight = glyphheight;
	p->hasglyph = 1;

    } else {
	p->glyphwidth = 0;
	p->glyphheight = 0;
	p->hasglyph = 0;
    }
    p->callbackfunc = '\0';
    p->panel = '\0';
    p->customptr = '\0';
    return p;
}

int glCompSetAddButton(glCompSet * s, glCompButton * p)
{
    s->buttoncount++;
    s->buttons =
	realloc(s->buttons, sizeof(glCompButton *) * s->buttoncount);
    s->buttons[s->buttoncount - 1] = p;
    p->parentset = s;

    return 1;
}

int glCompSetRemoveButton(glCompSet * s, glCompButton * p)
{
    int ind = 0;
    int found = 0;
    for (ind; ind < s->buttoncount ; ind++) {
	if ((s->buttons[ind] == p) && found == 0)
	    found = 1;
	if ((found == 1) &&(ind <= (s->buttoncount-1)))
	    s->buttons[ind] = s->buttons[ind + 1];
    }
    if (found) {
	free(p->caption);
	free(p);
	s->buttoncount--;
	s->buttons =
	    realloc(s->buttons, sizeof(glCompButton *) * s->buttoncount);

	return 1;
    }
    return 0;
}

int glCompDrawButton(glCompButton * p)
{
    float color_fac;
    float thickness = p->thickness;
    float fontx, fonty;
    if (!p->visible)
	return 0;
    if (p->panel) {
	p->pos.x = p->panel->pos.x + p->pos.x;
	p->pos.y = p->panel->pos.y + p->pos.y;
    }
    if (p->status == 1) {
	color_fac = GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS;
	glColor4f(p->color.R / (GLfloat) 1.2, p->color.G / (GLfloat) 1.2,
		  p->color.B / (GLfloat) 1.2, p->color.A);
	p->thickness = p->thickness / (GLfloat) 1.2;

    }

    else {
	color_fac = 1 / GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS;
	glColor4f(p->color.R, p->color.G, p->color.B, p->color.A);
	p->thickness = p->thickness * (GLfloat) 1.2;
    }
    if (!p->hasglyph) {
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//buttom thickness
	glColor4f(p->color.R * color_fac, p->color.G * color_fac,
		  p->color.B * color_fac, p->color.A);
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y, p->bevel);
	glVertex3f(p->pos.x, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//left thickness
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glEnd();

	glColor4f(p->color.R / color_fac, p->color.G / color_fac,
		  p->color.B / color_fac, p->color.A);
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//left thickness
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glEnd();
	//draw caption
	fontx =
	    (p->width - p->thickness * (GLfloat) 2 -
	     p->fontsize * strlen(p->caption) *
	     GLCOMPSET_FONT_SIZE_FACTOR) / (GLfloat) 2.0 + p->pos.x +
	    p->thickness;
	fonty =
	    (p->height - p->thickness * (GLfloat) 2 -
	     p->fontsize) / (GLfloat) 2.0 + p->pos.y + p->thickness;
	fontSize((int) p->fontsize);
//              fontColorA (p->fontcolor.R,p->fontcolor.B,p->fontcolor.G,p->fontcolor.A);
	fontColorA(0, 0, 0, 1);
	fontDrawString((int) fontx, (int) fonty, p->caption,
		       (int) (p->fontsize * strlen(p->caption) *
			      GLCOMPSET_FONT_SIZE_FACTOR));
    }
    //put glyph
    else {

	glEnable(GL_TEXTURE_2D);
	fontx =
	    (p->width - p->thickness * (GLfloat) 2 -
	     p->glyphwidth) / (GLfloat) 2.0 + p->pos.x + p->thickness;
	fonty =
	    (p->height - p->thickness * (GLfloat) 2 -
	     p->glyphheight) / (GLfloat) 2.0 + p->pos.y + p->thickness;
	glBindTexture(GL_TEXTURE_2D, p->glyph->id);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0f, 1.0f);
	glVertex3d(fontx, fonty, p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(1.0f, 1.0f);
	glVertex3d(fontx + p->glyph->w, fonty,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(1.0f, 0.0f);
	glVertex3d(fontx + p->glyph->w, fonty + p->glyph->h,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(0.0f, 0.0f);
	glVertex3d(fontx, fonty + p->glyph->h,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
//              glTexCoord2d(fontx,fonty); glVertex3d(fontx,fonty,p->bevel+GLCOMPSET_BEVEL_DIFF);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	if (p->status == 1) {
	    glColor4f(p->color.R * color_fac, p->color.G * color_fac,
		      p->color.B * color_fac, p->color.A / 2);
	    glBegin(GL_POLYGON);
	    glVertex3d(fontx - p->thickness, fonty - p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx + p->glyph->w + p->thickness,
		       fonty - p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx + p->glyph->w + p->thickness,
		       fonty + p->glyph->h + p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx - p->thickness,
		       fonty + p->glyph->h + p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glEnd();

	}

    }
    p->thickness = thickness;
    if (p->panel) {
	p->pos.x = p->pos.x - p->panel->pos.x;
	p->pos.y = p->pos.y - p->panel->pos.y;
    }

    return 1;


}
int glCompSetClick(glCompSet * s, int x, int y)
{

    if (s) {
	int ind = 0;
	float X, Y, Z;

	glCompDrawBegin();
	glCompSetGetPos(x, y, &X, &Y, &Z);
	glCompDrawEnd();


	s->clickedX = X;
	s->clickedY = Y;

	for (ind = 0; ind < s->buttoncount; ind++) {
	    if ((s->buttons[ind]->visible) && (s->buttons[ind]->enabled)) {
		if (glCompPointInButton(s->buttons[ind], X, Y)) {
		    if (s->buttons[ind]->groupid > -1)
			s->buttons[ind]->status = 1;
		}
	    }
	}
	return 1;
    } else
	return 0;
}

int glCompSetRelease(glCompSet * s, int x, int y)
{

    int ind = 0;
    if (s) {
	for (ind = 0; ind < s->buttoncount; ind++) {
	    if ((s->buttons[ind]->visible) && (s->buttons[ind]->enabled)) {
		if ((glCompPointInButton
		     (s->buttons[ind], s->clickedX, s->clickedY))) {
		    glCompButtonClick(s->buttons[ind]);
			break;
		}
	    }
	}
    }
    return 1;
}





int glCompPointInButton(glCompButton * p, float x, float y)
{
    float button_x, button_y;
    if (p->panel) {
	button_x = p->pos.x + p->panel->pos.x;
	button_y = p->pos.y + p->panel->pos.y;
    }

    if ((x >= button_x) &&
	(x <= button_x + p->width) &&
	(y >= button_y) && (y <= button_y + p->height))
	return 1;
    else
	return 0;

}

//converts screen location to opengl coordinates
void glCompSetGetPos(int x, int y, float *X, float *Y, float *Z)
{

    GLdouble wwinX;
    GLdouble wwinY;
    GLdouble wwinZ;
    GLdouble posX, posY, posZ;


    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
    float kts = 1;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    //draw a point  to a not important location to get window coordinates
    glBegin(GL_POINTS);
    glVertex3f(10.00, 10.00, 0.00);
    glEnd();
    gluProject(10.0, 10.0, 0.00, modelview, projection, viewport, &wwinX,
	       &wwinY, &wwinZ);
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport, &posX,
		 &posY, &posZ);

    *X = (float) posX;
    *Y = (float) posY;
    *Z = (float) posZ;
}


void glCompButtonClick(glCompButton * p)
{
    int ind;
    if (p->groupid > 0) {
	for (ind = 0; ind < ((glCompSet *) p->parentset)->buttoncount;
	     ind++) {
	    if (((glCompSet *) p->parentset)->buttons[ind]->groupid ==
		p->groupid)
		((glCompSet *) p->parentset)->buttons[ind]->status = 0;
	}
	p->status = 1;
    } else {
	if (p->groupid == -1) {
	    if (p->status == 0)
		p->status = 1;
	    else
		p->status = 0;
	} else
	    p->status = 0;
    }
    if (p->callbackfunc)
	p->callbackfunc(p);


}

glCompLabel *glCompLabelNew(GLfloat x, GLfloat y, GLfloat size, char *text)
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
    return p;
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

void glCompSetClear(glCompSet * s)
{
    int ind = 0;
    for (ind = 0; ind < s->buttoncount; ind++) {
	free(s->buttons[ind]->caption);
	free(s->buttons[ind]);
    }
    free(s->buttons);
    for (ind = 0; ind < s->labelcount; ind++) {
	free(s->labels[ind]->text);
	free(s->labels[ind]);
    }
    free(s->labels);
    for (ind = 0; ind < s->panelcount; ind++) {
	free(s->panels[ind]);
    }
    free(s->panels);
    free(s);
}
