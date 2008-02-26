#ifndef GLUTILS_H
#define GLUTILS_H
#include "windows.h"
#include <GL/gl.h>
#include <GL/glu.h>							


int GetFixedOGLPos(int x, int y,float kts,GLfloat* X,GLfloat* Y, GLfloat* Z);
int GetOGLPosRef(int x, int y,float* X,float* Y,float* Z);
float GetOGLDistance(int l);

#endif

