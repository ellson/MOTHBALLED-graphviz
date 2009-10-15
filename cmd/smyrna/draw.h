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

#ifndef DRAW_H
#define DRAW_H
#include "smyrnadefs.h"
#include <gtk/gtkgl.h>
#include "xdot.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "glcompfont.h"

#ifdef __cplusplus
extern "C" {
#endif
/* DRAWING FUNCTIONS 
 * these are opengl based xdot drawing functions 
 * topview drawings are not here
 */
    extern drawfunc_t OpFns[];
    extern void drawGraph(Agraph_t * g);
    void scanGraph(Agraph_t * g);
    void draw_selection_box(ViewInfo * view);
    void draw_magnifier(ViewInfo * view);
    void draw_fisheye_magnifier(ViewInfo * view);
    extern int randomize_color(glCompColor * c, int brightness);
    extern void drawCircle(float x, float y, float radius, float zdepth);
    extern glCompColor GetglCompColor(char *color);
    extern void drawBorders(ViewInfo * view);
    void drawEllipse(float xradius, float yradius, int angle1, int angle2);
    int draw_node_hintbox(GLfloat x, GLfloat y, GLfloat z, GLfloat fs,
			  char *text);
    void DrawBezier(GLfloat * xp, GLfloat * yp, GLfloat * zp, int filled,
		    int param);
	void draw_sphere(float x, float y, float z, float r);

	/*xdot drawing functions*/
	extern  void DrawBeziers(sdot_op* o, int param);
	extern void DrawEllipse(sdot_op * op, int param);
	extern void DrawPolygon(sdot_op * op, int param);
	extern void DrawPolyline(sdot_op * op, int param);
	extern void SetFillColor(sdot_op*  o, int param);
	extern void SetPenColor(sdot_op* o, int param);
	extern void SetStyle(sdot_op* o, int param);
	extern void SetFont(sdot_op * o, int param);
	extern void InsertImage(sdot_op * o, int param);
	extern void EmbedText(sdot_op * o, int param);

	typedef struct 
	{
		glCompColor color;
		float width;
	}xdotstyle;



	typedef struct 
	{
		glCompColor penColor;
		glCompColor fillColor;
		xdotstyle style;
    } xdotstate;	




#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
