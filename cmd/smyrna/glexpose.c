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
/*
	refreshes camera settings using view parameters such as pan zoom etc
	if a camera is selected viewport is switched to 3D
	params:ViewInfo	, global view variable defined in viewport.c
	return value:always 1
*/
int glupdatecamera(ViewInfo * view)
{

	glLoadIdentity();
	if (view->active_camera==-1)
	{
		gluLookAt(view->panx, view->pany, view->zoom * -1, view->panx,
			view->pany, 0.0, 0.0, 1.0, 0.0);
	}


	/*toggle to active camera*/
	else
	{
		/*gluLookAt(view->cameras[view->active_camera]->x,view->cameras[view->active_camera]->y,view->cameras[view->active_camera]->z,
		view->cameras[view->active_camera]->targetx,view->cameras[view->active_camera]->targety,view->cameras[view->active_camera]->targetz,
		view->cameras[view->active_camera]->camera_vectorx,
		view->cameras[view->active_camera]->camera_vectory,
		view->cameras[view->active_camera]->camera_vectorz);*/
		gluLookAt(view->panx, view->pany, view->zoom * -1, view->panx,
			view->pany, 0.0, 0.0, 1.0, 0.0);
			glRotatef(view->cameras[view->active_camera]->angley,1,0,0);
			glRotatef(view->cameras[view->active_camera]->anglex,0,1,0);


	}
	GetOGLPosRef(1, view->h - 5, &(view->clipX1), &(view->clipY1),
		 &(view->clipZ1));
    GetOGLPosRef(view->w - 1, 1, &(view->clipX2), &(view->clipY2),
		 &(view->clipZ2));
	if (view->active_camera==-1)
	{
		glScalef(1/view->zoom*-1,1/view->zoom*-1,1/view->zoom*-1);
	}
	else
		glScalef(1/view->cameras[view->active_camera]->r,1/view->cameras[view->active_camera]->r,1/view->cameras[view->active_camera]->r);
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
    if (!glupdatecamera(view))
	return 0;

	glexpose_grid(view);
    draw_fisheye_magnifier(view);
    draw_magnifier(view);
    glexpose_drawgraph(view);
    draw_selection_box(view);
    drawBorders(view);
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

	/*DEBUG*/

	return 1;
}

/*
	draws grid (little dots , with no use)
	params:ViewInfo	, global view variable defined in viewport.c
	return value:none
*/
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
	if (GD_TopView(view->g[view->activeGraph])) {
	    drawTopViewGraph(view->g[view->activeGraph]);	//view->Topview style dots and straight lines
	    glCompSetDraw(view->Topview->topviewmenu);
	} else
	    drawGraph(view->g[view->activeGraph]);	//xdot based drawing functions

	return 1;
    }
    return 0;
}
