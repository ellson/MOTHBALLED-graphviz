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
	int const WIDTH = 40;

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
	GLfloat winZ[1600];
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
//	printf ("%f %f %f\n",*X,*Y,*Z);
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
  double x, y, z;
} point;


typedef struct {
  point u, v;
} line;
typedef struct {
  point N;  /* normal */
  double d;  /* offset */
} plane;


static point add (point p, point q)
{
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
    return p;
}

static point sub (point p, point q)
{
    p.x -= q.x;
    p.y -= q.y;
    p.z -= q.z;
    return p;
}

static double dot (point p, point q)
{
    return (p.x*p.x + p.y*p.y + p.z*p.z); }

double len (point p)
{
    sqrt(dot(p,p));
}

point scale (double d, point p)
{
    p.x *= d;
    p.y *= d;
    p.z *= d;
    return p;
}

point normalize (point p)
{
   double d = len (p);

   return scale (1/d, p);
}

double dist (point p, point q)
{
    return (len (sub (p,q)));
}

point intersect (line l, plane J)
{
    double t = -(J.d + dot(l.u,J.N))/dot(l.v,J.N);
    return (add(l.u, scale(t,l.v)));
}

/*
 * Given a line l determined by two points a and b, and a 3rd point p:
 */
double point_to_line_dist (point a, point b, point p) {
    line l;
    plane J;
    point q;

    l.u = a;
    l.v = normalize (sub (b, a));

    J.N = l.v;
    J.d = -dot(p, l.v);

    q = intersect (l, J);

    return (dist (p, q));
}



typedef struct tagXYZ
{
    float X, Y, Z;
}
XYZ;

float Magnitude( XYZ *Point1, XYZ *Point2 )
{
    XYZ Vector;

    Vector.X = Point2->X - Point1->X;
    Vector.Y = Point2->Y - Point1->Y;
    Vector.Z = Point2->Z - Point1->Z;

    return (float)sqrt( Vector.X * Vector.X + Vector.Y * Vector.Y + Vector.Z * Vector.Z );
}

int DistancePointLine( XYZ *Point, XYZ *LineStart, XYZ *LineEnd, float *Distance )
{
    float LineMag;
    float U;
    XYZ Intersection;

    LineMag = Magnitude( LineEnd, LineStart );

    U = ( ( ( Point->X - LineStart->X ) * ( LineEnd->X - LineStart->X ) ) +
    ( ( Point->Y - LineStart->Y ) * ( LineEnd->Y - LineStart->Y ) ) +
    ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) ) /
    ( LineMag * LineMag );

    if( U < 0.0f || U > 1.0f )
        return 0;   // closest point does not fall within the line segment

    Intersection.X = LineStart->X + U * ( LineEnd->X - LineStart->X );
    Intersection.Y = LineStart->Y + U * ( LineEnd->Y - LineStart->Y );
    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    *Distance = Magnitude( Point, &Intersection );

    return 1;
}

/*void main( void )
{
    XYZ LineStart, LineEnd, Point;
    float Distance;


    LineStart.X =  50.0f; LineStart.Y =   80.0f; LineStart.Z =  300.0f;
    LineEnd.X   =  50.0f; LineEnd.Y   = -800.0f; LineEnd.Z   = 1000.0f;
    Point.X     =  20.0f; Point.Y     = 1000.0f; Point.Z     =  400.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance     );
    else
        printf( "closest point does not fall within line segment\n" );


    LineStart.X =  0.0f; LineStart.Y =   0.0f; LineStart.Z =  50.0f;
    LineEnd.X   =  0.0f; LineEnd.Y   =   0.0f; LineEnd.Z   = -50.0f;
    Point.X     = 10.0f; Point.Y     =  50.0f; Point.Z     =  10.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance     );
    else
        printf( "closest point does not fall within line segment\n" );
}*/

