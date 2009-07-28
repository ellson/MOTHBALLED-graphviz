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

	glColor4f((GLfloat)0,(GLfloat)0,(GLfloat)0,(GLfloat)0.001);
	glBegin(GL_POINTS);
    glVertex3f((GLfloat)-100.00, (GLfloat)-100.00, (GLfloat)1.00);
    glEnd();
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluProject(-100.0, -100.0, 1.00, modelview, projection, viewport, &wwinX,
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
    //glTranslatef (0.0,0.0,0.0);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    //draw a point  to a not important location to get window coordinates
	glColor4f((GLfloat)0,(GLfloat)0,(GLfloat)0,(GLfloat)0.001);

	glBegin(GL_POINTS);
    glVertex3f(-100.00, -100.00, 0.00);
    glEnd();
    gluProject(-100.0, -100.0, 0.00, modelview, projection, viewport, &wwinX,
	       &wwinY, &wwinZ);
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    gluUnProject(winX, winY, wwinZ, modelview, projection, viewport, &posX,
		 &posY, &posZ);

    *X = (float) posX;
    *Y = (float) posY;
    *Z = (float) posZ;
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	printf("==>(%d,%d,%d) -> (%f,%f,%f)\n",x,y,wwinZ,*X,*Y,*Z);


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




	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    //draw a point  to a not important location to get window coordinates
	glColor4f((GLfloat)0,(GLfloat)0,(GLfloat)0,(GLfloat)0.001);

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
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return ((float) (posXX - posX));

}
/*
	functions def: returns opengl coordinates of firt hit object by using screen coordinates
	x,y; 2D screen coordiantes (usually received from mouse events
	X,Y,Z; pointers to coordinates values to be calculated
	return value: no return value


*/

void to3D(int x, int y, GLfloat *X, GLfloat *Y,GLfloat *Z)
{
	int const WIDTH = 10;

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
	GLfloat winZ[100];
    GLdouble posX, posY, posZ;
	int idx;
	static float comp;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float) x;
    winY = (float) viewport[3] - (float) y;

	glReadPixels(x-WIDTH/2.0, (int)winY-WIDTH/2.0, WIDTH, WIDTH, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	comp=-9999999;
	for (idx=0;idx < WIDTH* WIDTH ; idx ++)
	{
		if ((winZ[idx] > comp) && (winZ[idx] < 1))
				comp = winZ[idx];
	}
	gluUnProject(winX,winY, comp,modelview, projection, viewport, &posX,
	       &posY, &posZ);

	*X = (GLfloat) posX;
    *Y = (GLfloat) posY;
    *Z = (GLfloat) posZ;
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return ;
	
	
	
	

}


int GetFixedOGLPoslocal(int x, int y, GLfloat * X, GLfloat * Y,
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



	glColor4f((GLfloat)0,(GLfloat)0,(GLfloat)0,(GLfloat)0.001);
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

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return 1;

}
void linear_interplotate (float x1,float y1,float x2,float y2,float x3,float* y3)
{

	float a,b;
	a=(y1-y2)/(x1-x2);
	b=y1-a*x1;
	*y3=a*x3+b;
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
#include <math.h>

typedef struct {
  point3f u, v;
} line;

typedef struct {
  point3f N;   /* normal */
  double d;  /* offset */
} plane;


static point3f add (point3f p, point3f q)
{
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
    return p;
}

static point3f sub (point3f p, point3f q)
{
    p.x -= q.x;
    p.y -= q.y;
    p.z -= q.z;
    return p;
}

static double dot (point3f p, point3f q)
{
    return (p.x*q.x + p.y*q.y + p.z*q.z); 
}

static double len (point3f p)
{
    return sqrt(dot(p,p));
}

static point3f scale (double d, point3f p)
{
    p.x *= d;
    p.y *= d;
    p.z *= d;
    return p;
}

static point3f blend (point3f p, point3f q, float m)
{
    point3f r;

    r.x = p.x + m * ( q.x - p.x );
    r.y = p.y + m * ( q.y - p.y );
    r.z = p.z + m * ( q.z - p.z );
    return r;
}

static point3f normalize (point3f p)
{
   double d = len (p);

   return scale (1/d, p);
}

static double dist (point3f p, point3f q)
{
    return (len (sub (p,q)));
}

static point3f intersect (line l, plane J)
{
    double t = -(J.d + dot(l.u,J.N))/dot(l.v,J.N);
    return (add(l.u, scale(t,l.v)));
}

/*
 * Given a line l determined by two points a and b, and a 3rd point p,
 * return the distance between the point and the line
 */
double point_to_line_dist (point3f p, point3f a, point3f b) {
    line l;
    plane J;
    point3f q;

    l.u = a;
    l.v = normalize (sub (b, a));

    J.N = l.v;
    J.d = -dot(p, l.v);

    q = intersect (l, J);

    return (dist (p, q));
}


/*
 * Given a line segment determined by two points a and b, and a 3rd point p,
 * return the distance between the point and the segment.
 * If the perpendicular from p to the line a-b is outside of the segment,
 * return the distance to the closer of a or b.
 */
double point_to_lineseg_dist (point3f p, point3f a, point3f b)
{
    float U;
    point3f q;
    point3f ba = sub (b, a);
    point3f pa = sub (p, a);

    U = dot (pa, ba)/dot (ba, ba);

    if (U > 1)
	q = b;
    else if (U < 0)
	q = a;
    else 
	q = blend (a, b, U);

    return dist(p, q);

}

#ifdef DEBUG
void main( void )
{
    point3f LineStart, LineEnd, Point;
    float Distance;


    LineStart.x =  50.0f; LineStart.y =   80.0f; LineStart.z =  300.0f;
    LineEnd.x   =  50.0f; LineEnd.y   = -800.0f; LineEnd.z   = 1000.0f;
    Point.x     =  20.0f; Point.y     = 1000.0f; Point.z     =  400.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance     );
    else
        printf( "closest point does not fall within line segment\n" );


    LineStart.x =  0.0f; LineStart.y =   0.0f; LineStart.z =  50.0f;
    LineEnd.x   =  0.0f; LineEnd.y   =   0.0f; LineEnd.z   = -50.0f;
    Point.x     = 10.0f; Point.y     =  50.0f; Point.z     =  10.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance     );
    else
        printf( "closest point does not fall within line segment\n" );
}
#endif

