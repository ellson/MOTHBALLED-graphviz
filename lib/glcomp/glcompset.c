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
#include "memory.h"
#include "glcomppanel.h"
#include "glcomplabel.h"
#include "glcompbutton.h"

//#ifdef WIN32
/*char *
mystrdup(const char *string)
{
     char       *nstr;
 
     nstr = (char *) malloc(strlen(string) + 1);
     if (nstr)
         strcpy(nstr, (char*)string);
     return nstr;
}
#define strdup mystrdup
#endif*/
//converts screen location to opengl coordinates
static void glCompSetGetPos(int x, int y, float *X, float *Y, float *Z)
{
    GLdouble wwinX;
    GLdouble wwinY;
    GLdouble wwinZ;
    GLdouble posX, posY, posZ;


    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
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

void glCompDrawBegin(void)	//pushes a gl stack 
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

void glCompDrawEnd(void)	//pops the gl stack 
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}










static int glCompSetDrawPanels(glCompSet * s)
{
    int ind = 0;
    for (; ind < s->panelcount; ind++) {
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
    for (; ind < s->panelcount; ind++) {
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
    for (; ind < s->panelcount; ind++) {
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
static int glCompPointInButton(glCompButton * p, float x, float y)
{
    int kts, kts2;
    /* GLfloat tempX,tempY; */
    GLfloat h, h2;		/*container widget height */
    /* float color_fac; */
    /* float thickness = p->thickness; */
    /* float fontx, fonty; */
    /* GLfloat fontwidth; */
    float button_x, button_y;

    if (p->orientation == 1) {
	kts = 1;
	h = 0;
    } else {
	kts = -1;
	h = ((glCompSet *) p->parentset)->h;
    }
    if (p->panel->orientation == 1) {
	kts2 = 1;
	h2 = 0;
    } else {
	kts2 = -1;
	h2 = ((glCompSet *) p->panel->parentset)->h;
    }
    if (!p->visible)
	return 0;
    if (p->panel) {
	button_x = p->panel->pos.x + p->pos.x;
	button_y = p->panel->pos.y * kts2 * kts + h2 + p->pos.y - h;
	if (p->panel->orientation == 0)
	    button_y = button_y - p->panel->height;
    }

    if ((x >= button_x) && (x <= button_x + p->width) && (y >= button_y)
	&& (y <= button_y + p->height))
	return 1;
    else
	return 0;

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





void glCompSetClear(glCompSet * s)
{
    int ind = 0;
    for (ind = 0; ind < s->buttoncount; ind++) {
	/*if (s->buttons[ind]->caption)
	   free(s->buttons[ind]->caption);
	   free(s->buttons[ind]); */
	glCompSetRemoveButton(s, s->buttons[ind]);
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

glCompSet *glCompSetNew(int w, int h)
{
    glCompSet *s = NEW(glCompSet);
    s->w = (GLfloat) w;
    s->h = (GLfloat) h;
    s->groupCount = 0;
    return s;
}

int glCompSetDraw(glCompSet * s)
{
    glCompDrawBegin();
    glCompSetDrawPanels(s);
    glCompDrawEnd();
    return 1;
}

void glcompsetUpdateBorder(glCompSet * s, int w, int h)
{
    if (w > 0 && h > 0) {
	s->w = (GLfloat) w;
	s->h = (GLfloat) h;
    }
}
extern int glcompsetGetGroupId(glCompSet * s)
{
    return s->groupCount;
}
extern int glcompsetNextGroupId(glCompSet * s)
{
    int rv = s->groupCount;
    s->groupCount++;
    return rv;
}


#if 0
static void change_fonts(glCompSet * s, const texFont_t * sourcefont)
{
    int ind;

    for (ind = 0; ind < s->buttoncount; ind++) {
	copy_font((s->buttons[ind]->font), sourcefont);
    }
    for (ind = 0; ind < s->labelcount; ind++) {
	copy_font((s->labels[ind]->font), sourcefont);

    }
    for (ind = 0; ind < s->panelcount; ind++) {
	copy_font((s->panels[ind]->font), sourcefont);
    }
}
#endif
