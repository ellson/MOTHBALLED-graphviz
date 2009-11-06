/* $Id$Revision: */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "glexpose.h"
#include "draw.h"
#include "topview.h"
#include "glutils.h"
#include "topview.h"
#include "topfisheyeview.h"
#include "gui/toolboxcallbacks.h"
#include "arcball.h"

void drawRotatingAxis(void);
void draw_cube()
{

    glBegin(GL_QUADS);		// Draw The Cube Using quads
    glColor3f(0.0f, 100.0f, 0.0f);	// Color Blue
    glVertex3f(100.0f, 100.0f, -100.0f);	// Top Right Of The Quad (Top)
    glVertex3f(-100.0f, 100.0f, -100.0f);	// Top Left Of The Quad (Top)
    glVertex3f(-100.0f, 100.0f, 100.0f);	// Bottom Left Of The Quad (Top)
    glVertex3f(100.0f, 100.0f, 100.0f);	// Bottom Right Of The Quad (Top)
    glColor3f(100.0f, 0.5f, 0.0f);	// Color Orange
    glVertex3f(100.0f, -100.0f, 100.0f);	// Top Right Of The Quad (Bottom)
    glVertex3f(-100.0f, -100.0f, 100.0f);	// Top Left Of The Quad (Bottom)
    glVertex3f(-100.0f, -100.0f, -100.0f);	// Bottom Left Of The Quad (Bottom)
    glVertex3f(100.0f, -100.0f, -100.0f);	// Bottom Right Of The Quad (Bottom)
    glColor3f(100.0f, 0.0f, 0.0f);	// Color Red    
    glVertex3f(100.0f, 100.0f, 100.0f);	// Top Right Of The Quad (Front)
    glVertex3f(-100.0f, 100.0f, 100.0f);	// Top Left Of The Quad (Front)
    glVertex3f(-100.0f, -100.0f, 100.0f);	// Bottom Left Of The Quad (Front)
    glVertex3f(100.0f, -100.0f, 100.0f);	// Bottom Right Of The Quad (Front)
    glColor3f(100.0f, 100.0f, 0.0f);	// Color Yellow
    glVertex3f(100.0f, -100.0f, -100.0f);	// Top Right Of The Quad (Back)
    glVertex3f(-100.0f, -100.0f, -100.0f);	// Top Left Of The Quad (Back)
    glVertex3f(-100.0f, 100.0f, -100.0f);	// Bottom Left Of The Quad (Back)
    glVertex3f(100.0f, 100.0f, -100.0f);	// Bottom Right Of The Quad (Back)
    glColor3f(0.0f, 0.0f, 100.0f);	// Color Blue
    glVertex3f(-100.0f, 100.0f, 100.0f);	// Top Right Of The Quad (Left)
    glVertex3f(-100.0f, 100.0f, -100.0f);	// Top Left Of The Quad (Left)
    glVertex3f(-100.0f, -100.0f, -100.0f);	// Bottom Left Of The Quad (Left)
    glVertex3f(-100.0f, -100.0f, 100.0f);	// Bottom Right Of The Quad (Left)
    glColor3f(100.0f, 0.0f, 100.0f);	// Color Violet
    glVertex3f(100.0f, 100.0f, -100.0f);	// Top Right Of The Quad (Right)
    glVertex3f(100.0f, 100.0f, 100.0f);	// Top Left Of The Quad (Right)
    glVertex3f(100.0f, -100.0f, 100.0f);	// Bottom Left Of The Quad (Right)
    glVertex3f(100.0f, -100.0f, -100.0f);	// Bottom Right Of The Quad (Right)
    glEnd();
}



void drawRotatingAxis(void)
{
    static GLUquadricObj *quadratic = (GLUquadricObj *) 0;
	float AL = 45;

    if (get_mode(view) != MM_ROTATE)
	    return;

    if (!quadratic) {
	quadratic = gluNewQuadric();	// Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals
//              gluQuadricTexture(quadratic, GL_TRUE);                                                  // Create Texture Coords
	gluQuadricDrawStyle(quadratic, GLU_LINE);


    }

	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(view->arcball->Transform.M);	/*arcball transformations , experimental */
	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(1, 1, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(0, AL, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(AL, 0, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, AL);

	glEnd();
	glColor4f(0, 1, 0, 0.3);
	gluSphere(quadratic, AL, 20, 20);
	glLineWidth(1);
	glPopMatrix();


}



/*
	refreshes camera settings using view parameters such as pan zoom etc
	if a camera is selected viewport is switched to 3D
	params:ViewInfo	, global view variable defined in viewport.c
	return value:always 1
*/
int glupdatecamera(ViewInfo * view)
{
    if (view->active_camera == -1)
	glTranslatef(-view->panx, -view->pany, view->panz);


    /*toggle to active camera */
    else {
	glMultMatrixf(view->arcball->Transform.M);	/*arcball transformations , experimental */
	glTranslatef(-view->cameras[view->active_camera]->targetx,
		     -view->cameras[view->active_camera]->targety, 0);
    }

    GetOGLPosRef(1, view->h - 5, &(view->clipX1), &(view->clipY1),
		 &(view->clipZ1));
    GetOGLPosRef(view->w - 1, 1, &(view->clipX2), &(view->clipY2),
		 &(view->clipZ2));

    if (view->active_camera == -1) {
	glScalef(1 / view->zoom * -1, 1 / view->zoom * -1,
		 1 / view->zoom * -1);
    } else {
	glScalef(1 / view->cameras[view->active_camera]->r,
		 1 / view->cameras[view->active_camera]->r,
		 1 / view->cameras[view->active_camera]->r);
    }


    return 1;
}

/*
	main gl expose ,any time sreen needs to be redrawn, this function is called by gltemplate
	,all drawings are initialized in this function
	params:ViewInfo	, global view variable defined in viewport.c
	return value:0 if something goes wrong with GL 1 , otherwise
*/
int glexpose_main(ViewInfo * view)
{
    static int doonce = 0;
    if (!glupdatecamera(view))
	return 0;

    if (view->activeGraph >= 0) {
	if (!doonce) {
	    doonce = 1;
	    btnToolZoomFit_clicked(NULL, NULL);
	    btnToolZoomFit_clicked(NULL, NULL);
	}
    }
    else
	return;

    glexpose_grid(view);
    draw_fisheye_magnifier(view);
    draw_magnifier(view);
    glexpose_drawgraph(view);
    drawBorders(view);
//      drawRotatingTools();
//      draw_cube();
    drawRotatingAxis();
//              draw_stuff();
//      test_color_pallete();



//      drawtestpoly();
     /*DEBUG*/
/*	if (view->mouse.mouse_mode == MM_PAN)
	{
		glBegin(GL_LINE_STRIP);
		glColor4f((GLfloat) 1, (GLfloat) 0.0, (GLfloat) 0.0,
			(GLfloat) 1);
		glVertex3f((GLfloat) view->GLx, (GLfloat) view->GLy,
			(GLfloat) 0.001);
		glVertex3f((GLfloat) view->GLx2, (GLfloat) view->GLy2,
			(GLfloat) 0.001);


		glEnd();
	}*/
	 /*DEBUG*/ return 1;
}

/*
	draws grid (little dots , with no use)
	params:ViewInfo	, global view variable defined in viewport.c
	return value:none
*/
void drawtestpoly(void)
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE , GL_DECAL);
//      glBindTexture(GL_TEXTURE_2D,view->widgets->fontset->fonts[view->widgets->fontset->activefont]->texId);
    glBindTexture(GL_TEXTURE_2D, 1);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.0, 256.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(256.0, 256.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(256.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void glexpose_grid(ViewInfo * view)
{
    //drawing grids
    float x, y;
    if (view->gridVisible) {
	glPointSize(1);
	glBegin(GL_POINTS);
	glColor4f(view->gridColor.R, view->gridColor.G, view->gridColor.B,
		  view->gridColor.A);
	for (x = view->bdxLeft; x <= view->bdxRight;
	     x = x + view->gridSize) {
	    for (y = view->bdyBottom; y <= view->bdyTop;
		 y = y + view->gridSize) {
		glVertex3f(x, y, 0);
	    }
	}
	glEnd();
    }
}

/*
	draws active graph depending on graph type
	params:ViewInfo	, global view variable defined in viewport.c
	return value:1 if there is a graph to draw else 0 
*/
int glexpose_drawgraph(ViewInfo * view)
{

    if (view->activeGraph > -1) {
//              if (GD_TopView(view->g[view->activeGraph])) 
//              {
	if (!view->Topview->is_top_fisheye)
	    drawTopViewGraph(view->g[view->activeGraph]);	//view->Topview style dots and straight lines
	else {
	    drawtopologicalfisheye(view->Topview);
	}

//              }
//              else
//                      drawGraph(view->g[view->activeGraph]);  //xdot based drawing functions
	glCompSetDraw(view->widgets);
	return 1;
    }
    return 0;
}

/*
void drawRotatingTools(void)
{
    float x, y;
    float x1, y1, z1;
    float x2, y2, z2;
    float R1, R2;
    if ((view->mouse.mouse_mode == MM_ROTATE)
	&& (view->active_camera >= 0)) {
	R1 = 25;
	R2 = 200;
	glCompDrawBegin();
	GetOGLPosRef(1, view->h - 5, &x1, &y1, &z1);
	GetOGLPosRef(view->w - 1, 1, &x2, &y2, &z2);
	x = (x2 - x1) / (float) 2.0;
	y = (y2 - y1) / (float) 2.0;
	glTranslatef(x, y, 0);
	if ((view->mouse.rotate_axis == MOUSE_ROTATE_X)
	    || (view->mouse.rotate_axis == MOUSE_ROTATE_XY)) {
	    glLineWidth(2);
	    glColor4f(0, 1, 0, 0.5);
	} else {
	    glLineWidth(1);
	    glColor4f(1, 0, 0, 0.5);
	}
	drawEllipse(R1, R2, 90, 270);
	if ((view->mouse.rotate_axis == MOUSE_ROTATE_Y)
	    || (view->mouse.rotate_axis == MOUSE_ROTATE_XY)) {
	    glLineWidth(2);
	    glColor4f(0, 1, 0, 0.5);
	} else {
	    glLineWidth(1);
	    glColor4f(1, 0, 0, 0.5);
	}
	drawEllipse(R2, R1, 0, 180);
	if (view->mouse.rotate_axis == MOUSE_ROTATE_Z) {
	    glLineWidth(2);
	    glColor4f(0, 1, 0, 0.5);
	} else {
	    glLineWidth(1);
	    glColor4f(1, 0, 0, 0.5);
	}

	drawEllipse(R2, R2, 0, 360);
	glCompDrawEnd();
    }

}
*/