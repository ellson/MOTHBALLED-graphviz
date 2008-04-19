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

#include "glutils.h"
/* #include "glexpose.h" */

/* at given depth value, tranforms 2d Window location to 3d gl coords*/
int GetFixedOGLPos(int x, int y, float kts, GLfloat * X, GLfloat * Y,
		   GLfloat * Z)
{
    GLdouble wwinX;
    GLdouble wwinY;
    GLdouble wwinZ;

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
    GLdouble posX, posY, posZ;

    glBegin(GL_POINTS);
    glVertex3f(10.00, 10.00, 1.00);
    glEnd();
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluProject(10.0, 10.0, 1.00, modelview, projection, viewport, &wwinX,
	       &wwinY, &wwinZ);

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport, &posX,
		 &posY, &posZ);
    *X = (GLfloat) posX;
    *Y = (GLfloat) posY;
    *Z = (GLfloat) posZ;

    return 1;

}

/*transforms 2d windows location to 3d gl coords but depth is calculated unlike the previous function*/
int GetOGLPosRef(int x, int y, float *X, float *Y, float *Z)
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
    //glTranslatef (0.0,0.0,0.0);
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
    return 1;

}


float GetOGLDistance(int l)
{

    int x, y;
    GLdouble wwinX;
    GLdouble wwinY;
    GLdouble wwinZ;
    GLdouble posX, posY, posZ;
    GLdouble posXX, posYY, posZZ;



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
    glVertex3f(10.00, 10.00, 1.00);
    glEnd();
    gluProject(10.0, 10.0, 1.00, modelview, projection, viewport, &wwinX,
	       &wwinY, &wwinZ);
    x = 50;
    y = 50;
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport, &posX,
		 &posY, &posZ);
    x = x + l;
    y = 50;
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport,
		 &posXX, &posYY, &posZZ);
    return ((float) (posXX - posX));

}
int GetFixedOGLPoslocal(int x, int y, float kts, GLfloat * X, GLfloat * Y,
		   GLfloat * Z)
{
    GLdouble wwinX;
    GLdouble wwinY;
    GLdouble wwinZ;

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
    GLdouble posX, posY, posZ;
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

	glBegin(GL_POINTS);
    glVertex3f(10.00, 10.00, 0.00);
    glEnd();

	gluProject(10.0, 10.0, 1.00, modelview, projection, viewport, &wwinX,
	       &wwinY, &wwinZ);

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport, &posX,
		 &posY, &posZ);
	*X = (GLfloat) posX;
    *Y = (GLfloat) posY;
    *Z = (GLfloat) posZ;

    return 1;

}

#if 0
int glreversecamera(ViewInfo * view)
{

	glLoadIdentity();
	if (view->active_camera==-1)
	{
		gluLookAt(view->panx, view->pany, 20, view->panx,
			view->pany, 0.0, 0.0, 1.0, 0.0);
		glScalef(1*view->zoom*-1,1*view->zoom*-1,1*view->zoom*-1);
	}
	else
	{
		glScalef(1*view->cameras[view->active_camera]->r,1*view->cameras[view->active_camera]->r,1*view->cameras[view->active_camera]->r);

	}

	return 1;
}

#endif

