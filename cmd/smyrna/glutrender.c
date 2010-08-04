/* $Id$ $Revision$ */
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

#include "glutrender.h"
#include "viewport.h"
#include "arcball.h"
#include "appmouse.h"
#include "glexpose.h"


    /*call backs */

static float begin_x = 0.0;
static float begin_y = 0.0;
static float dx = 0.0;
static float dy = 0.0;

/*mouse mode mapping funvtion from glut to glcomp*/
static glMouseButtonType getGlCompMouseType(int n)
{
    switch (n) {
    case GLUT_LEFT_BUTTON:
	return glMouseLeftButton;
    case GLUT_RIGHT_BUTTON:
	return glMouseMiddleButton;
    case GLUT_MIDDLE_BUTTON:
	return glMouseRightButton;

    default:
	return glMouseLeftButton;
    }
}


void cb_glutreshape(int width, int height)
{
    /* static int doonce=0; */
    int vPort[4];
    float aspect;
    view->w = width;
    view->h = height;
    if (view->widgets)
	glcompsetUpdateBorder(view->widgets, view->w, view->h);
    glViewport(0, 0, view->w, view->h);
    /* get current viewport */
    glGetIntegerv(GL_VIEWPORT, vPort);
    /* setup various opengl things that we need */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    init_arcBall(view->arcball, (GLfloat) view->w, (GLfloat) view->h);
    if (view->w > view->h) {
	aspect = (float) view->w / (float) view->h;
	glOrtho(-aspect * GL_VIEWPORT_FACTOR, aspect * GL_VIEWPORT_FACTOR,
		GL_VIEWPORT_FACTOR * -1, GL_VIEWPORT_FACTOR, -1500, 1500);
    } else {
	aspect = (float) view->h / (float) view->w;
	glOrtho(GL_VIEWPORT_FACTOR * -1, GL_VIEWPORT_FACTOR,
		-aspect * GL_VIEWPORT_FACTOR, aspect * GL_VIEWPORT_FACTOR,
		-1500, 1500);
    }

    glMatrixMode(GL_MODELVIEW);
	/*** OpenGL END ***/

}
void cb_glutdisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(view->bgColor.R, view->bgColor.G, view->bgColor.B, view->bgColor.A);
    glLoadIdentity();
    glexpose_main(view);	//draw all stuff
    glutSwapBuffers();
    if (view->initFile) {
	view->initFile = 0;
	if (view->activeGraph == 0)
	    close_graph(view, 0);
	add_graph_to_viewport_from_file(view->initFileName);
    }

}
void cb_mouseclick(int button, int state,int x, int y)
{
    Agraph_t* g;

    if (view->g == 0) return;
    g=view->g[view->activeGraph];
    begin_x = (float) x;
    begin_y = (float) y;
    if(state==GLUT_DOWN)
    {
	view->widgets->common.functions.mousedown((glCompObj*)view->widgets,(GLfloat) x,(GLfloat)y,getGlCompMouseType(button));
	if (button ==  GLUT_LEFT_BUTTON)
	    appmouse_left_click_down(view,x,y);
        if (button ==  GLUT_RIGHT_BUTTON)
	    appmouse_right_click_down(view,x,y);
	if (button ==  GLUT_MIDDLE_BUTTON)
	    appmouse_middle_click_down(view,x,y);
    }
    else
    {
	view->FontSizeConst = GetOGLDistance(14);
	view->arcball->isDragging = 0;
	view->widgets->common.functions.mouseup((glCompObj*)view->widgets,(GLfloat) x,(GLfloat) y,getGlCompMouseType(button));

	if (button ==  GLUT_LEFT_BUTTON)
	    appmouse_left_click_up(view,x,y);
	if (button ==  GLUT_LEFT_BUTTON)
	    appmouse_right_click_up(view,x,y);
	if (button ==  GLUT_MIDDLE_BUTTON)
	    appmouse_middle_click_up(view,x,y);
	dx = 0.0;
	dy = 0.0;
    }
    cb_glutdisplay();

}
void cb_mouseover(int x,int y)/*no mouse click only mouse pointer moving on context*/
{








}
void cb_mouseover2(int X,int Y)/*mouse moving witha button clicked (dragging)*/
{

    float x = (float) X;
    float y = (float) Y;

    if (view->widgets)
	view->widgets->common.functions.mouseover((glCompObj*)view->widgets, (GLfloat) x,(GLfloat) y);

    dx = x - begin_x;
    dy = y - begin_y;
    view->mouse.dragX = dx;
    view->mouse.dragY = dy;
    appmouse_move(view,x,y);

    if((view->mouse.t==glMouseLeftButton) && (view->mouse.down)  )
	appmouse_left_drag(view,x,y);
    if((view->mouse.t==glMouseRightButton) && (view->mouse.down))
	appmouse_right_drag(view,x,y);
    if((view->mouse.t==glMouseMiddleButton) && (view->mouse.down))
	appmouse_middle_drag(view,x,y);
    begin_x = x;
    begin_y = y;
    cb_glutdisplay();

}


void sm_glutinit(int w,int h,int full)
{
    glutInitWindowSize(512,512);
 glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGBA|GLUT_ALPHA|GLUT_STENCIL);


//    glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow("The glut hello world program");
    glutDisplayFunc(cb_glutdisplay);
    glutReshapeFunc(cb_glutreshape);
    glutMouseFunc(cb_mouseclick);
    glutMotionFunc(cb_mouseover2);
    glutFullScreen();

    glClearColor(view->bgColor.R, view->bgColor.G, view->bgColor.B, view->bgColor.A);	//background color
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    glFrontFace(GL_CW);
// glEnable (GL_LIGHTING);
// glEnable (GL_LIGHT0);
//  glEnable (GL_AUTO_NORMAL);
//  glEnable (GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH);
//  glEnable(GL_LINE_SMOOTH);



/*    *   7.1 glutDisplayFunc
    * 7.2 glutOverlayDisplayFunc
    * 7.3 glutReshapeFunc
    * 7.4 glutKeyboardFunc
    * 7.5 glutMouseFunc
    * 7.6 glutMotionFunc, glutPassiveMotionFunc
    * 7.7 glutVisibilityFunc
    * 7.8 glutEntryFunc
    * 7.9 glutSpecialFunc
    * 7.10 glutSpaceballMotionFunc
    * 7.11 glutSpaceballRotateFunc
    * 7.12 glutSpaceballButtonFunc
    * 7.13 glutButtonBoxFunc
    * 7.14 glutDialsFunc
    * 7.15 glutTabletMotionFunc
    * 7.16 glutTabletButtonFunc
    * 7.17 glutMenuStatusFunc
    * 7.18 glutIdleFunc
    * 7.19 glutTimerFunc 
 */
    glutMainLoop(); // Infinite event loop
}

