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

/*

XDOT DRAWING FUNCTIONS, maybe need to move them somewhere else
		for now keep them at the bottom
*/
#include "draw.h"
#include "drawxdot.h"
#include "topview.h"
#include "colorprocs.h"
#include "glutils.h"
#include "math.h"
#include "selection.h"
#include "xdot.h"
#include "viewport.h"
#include "topfisheyeview.h"

static float globalz = 0.0;

#ifdef UNUSED
//delta values
static float dx = 0.0;
static float dy = 0.0;

static void set_options(sdot_op * op, int param)
{

    if ((param == 1) && (view->mouse.mouse_mode == 10) && (view->mouse.mouse_down == 1))	//selected, if there is move, move it
    {
	dx = view->GLx - view->GLx2;
	dy = view->GLy - view->GLy2;
    } else {
	dx = 0;
	dy = 0;
    }

}

static void SetFont(xdot_op * op, int param)
{
    //activate the right font
    view->fontset->activefont = add_font(view->fontset, op->u.font.name);	//load or set active font
    view->FontSize = (int) op->u.font.size;
}

static void relocate_spline(sdot_op * sop, int param)
{
    Agedge_t *e;
    Agnode_t *tn;		//tail node
    Agnode_t *hn;		//head node
    int i = 0;
    xdot_op *op = &sop->op;
    if (AGTYPE(sop->obj) == AGEDGE) {
	e = sop->obj;
	tn = agtail(e);
	hn = aghead(e);
	if ((OD_Selected(hn) == 1) && (OD_Selected(tn) == 0)) {
	    set_options(sop, 1);
	    for (i = 1; i < op->u.bezier.cnt - 1; i = i + 1) {
		if ((dx != 0) || (dy != 0)) {
		    op->u.bezier.pts[i].x =
			op->u.bezier.pts[i].x -
			(int) (dx * (float) i /
			       (float) (op->u.bezier.cnt));
		    op->u.bezier.pts[i].y =
			op->u.bezier.pts[i].y -
			(int) (dy * (float) i /
			       (float) (op->u.bezier.cnt));
		}
	    }
	    if ((dx != 0) || (dy != 0)) {
		op->u.bezier.pts[op->u.bezier.cnt - 1].x =
		    op->u.bezier.pts[op->u.bezier.cnt - 1].x - (int) dx;
		op->u.bezier.pts[op->u.bezier.cnt - 1].y =
		    op->u.bezier.pts[op->u.bezier.cnt - 1].y - (int) dy;
	    }
	} else if ((OD_Selected(hn) == 0) && (OD_Selected(tn) == 1)) {
	    set_options(sop, 1);
	    for (i = op->u.bezier.cnt - 1; i > 0; i = i - 1) {
		if ((dx != 0) || (dy != 0)) {
		    op->u.bezier.pts[i].x =
			op->u.bezier.pts[i].x -
			(int) (dx * (float) (op->u.bezier.cnt - i) /
			       (float) (op->u.bezier.cnt));
		    op->u.bezier.pts[i].y =
			op->u.bezier.pts[i].y -
			(int) (dy * (float) (op->u.bezier.cnt - i) /
			       (float) (op->u.bezier.cnt));
		}
	    }
	    if ((dx != 0) || (dy != 0)) {
		op->u.bezier.pts[0].x = op->u.bezier.pts[0].x - (int) dx;
		op->u.bezier.pts[0].y = op->u.bezier.pts[0].y - (int) dy;
	    }
	} else if ((OD_Selected(hn) == 1) && (OD_Selected(tn) == 1)) {
	    set_options(sop, 1);
	    for (i = 0; i < op->u.bezier.cnt; i = i + 1) {
		if ((dx != 0) || (dy != 0)) {
		    op->u.bezier.pts[i].x =
			op->u.bezier.pts[i].x - (int) dx;
		    op->u.bezier.pts[i].y =
			op->u.bezier.pts[i].y - (int) dy;
		}
	    }
	}
    }
}

static void DrawBeziers(xdot_op * op, int param)
{
    //SEND ALL CONTROL POINTS IN 3D ARRAYS

    GLfloat tempX[4];
    GLfloat tempY[4];
    GLfloat tempZ[4];
    int temp = 0;
    int filled;
    int i = 0;
//    SelectBeziers((sdot_op *) op);
    relocate_spline((sdot_op *) op, param);
    if (op->kind == xd_filled_bezier)
	filled = 1;
    else
	filled = 0;

    for (i = 0; i < op->u.bezier.cnt; i = i + 1) {
	if (temp == 4) {
	    DrawBezier(tempX, tempY, tempZ, filled, param);
	    tempX[0] = (GLfloat) op->u.bezier.pts[i - 1].x;
	    tempY[0] = (GLfloat) op->u.bezier.pts[i - 1].y;
	    tempZ[0] = (GLfloat) op->u.bezier.pts[i - 1].z;
	    temp = 1;
	    tempX[temp] = (GLfloat) op->u.bezier.pts[i].x;
	    tempY[temp] = (GLfloat) op->u.bezier.pts[i].y;
	    tempZ[temp] = (GLfloat) op->u.bezier.pts[i].z;
	    temp = temp + 1;
	} else {
	    tempX[temp] = (GLfloat) op->u.bezier.pts[i].x;
	    tempY[temp] = (GLfloat) op->u.bezier.pts[i].y;
	    tempZ[temp] = (GLfloat) op->u.bezier.pts[i].z;
	    temp = temp + 1;
	}
    }
    DrawBezier(tempX, tempY, tempZ, filled, param);
}
#endif

/*function to load .raw files*/
#ifdef UNUSED
static void
load_raw_texture(char *file_name, int width, int height, int depth,
		 GLenum colour_type, GLenum filter_type)
{
    //Line 3 creates a pointer to an (as yet unallocated) array of gl unsigned bytes - 
    //this will store our image until we've passed it to opengl.  Line 4 stores the file pointer, don't worry about that. 
    GLubyte *raw_bitmap;
    FILE *file;

    //The if statement from line 6-10 opens the file and reports an error if it doesn't exist.   

    if ((file = fopen(file_name, "rb")) == NULL) {
	printf("File Not Found : %s\n", file_name);
	exit(1);
    }
    //Line 11 allocates the correct number of bytes for the size and depth of the image.  Remember, our image depth will usually be 3 -- one 'channel' each for red, green and blue values.   
    //Lines 13-18 check if the memory was allocated correctly and quit the program if there was a problem. 

    raw_bitmap = N_GNEW(width * height * depth, GLubyte);

    if (raw_bitmap == NULL) {
	printf("Cannot allocate memory for texture\n");
	fclose(file);
	exit(1);
    }
    //Line 19 reads the required number of bytes from the file and places them into our glubyte array.  Line 20 closes the close as it isn't required anymore.   
    fread(raw_bitmap, width * height * depth, 1, file);
    fclose(file);

    //Lines 22-25 set the texture's mapping type and environment settings.  GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER are enumerands that let us change the way in which opengl magnifies and minifies the texture.  If we passed GL_LINEAR to the function, our texture would be interpolated using bilinear filtering (in other words, it'd appear smoothed).  If we passed GL_NEAREST then no smoothing would occur.  By passing GL_MODULATE to the texture environment function, we tell opengl to blend the texture with the base colour of the object.  Had we specified GL_DECAL or GL_REPLACE then the base colour (and therefore our lighting effect) would be replaced purely with the colours of the texture. 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //Line 27 passes the image to opengl using the function gluBuild2DMipmaps.
    //Read a graphics book for a description of mipmaps.
    //This function will also resize the texture map if it doesn't conform 
    //to the opengl restriction of height and width values being a power of 2.   
    gluBuild2DMipmaps(GL_TEXTURE_2D, colour_type, width, height,
		      colour_type, GL_UNSIGNED_BYTE, raw_bitmap);

    //We don't need our texture in the temporary array anymore as it has been passed to opengl.
    //We use the standard C library call 'free' to return the allocated memory.  
    free(raw_bitmap);
}
#endif


static void drawXdot(xdot * xDot, int param, void *p)
{
    int id;
    sdot_op *ops = (sdot_op *) (xDot->ops);
    sdot_op *op;
    //to avoid the overlapping , z is slightly increased for each xdot of a particular object
    if (AGTYPE(p) == AGEDGE)
	globalz = 1;
    else
	globalz = 0;

    for (id = 0; id < xDot->cnt; id++) {
	globalz += GLOBAL_Z_OFFSET;
	op = ops + id;
	op->obj = p;
	op->op.drawfunc(&(op->op), param);
    }
}



static void drawXdotwithattr(void *p, char *attr, int param)
{
    xdot *xDot;
    if ((xDot = parseXDotF(agget(p, attr), OpFns, sizeof(sdot_op)))) {
	drawXdot(xDot, param, p);
	freeXDot(xDot);
    }
}

static void drawXdotwithattrs(void *e, int param)
{
    drawXdotwithattr(e, "_draw_", param);
    drawXdotwithattr(e, "_ldraw_", param);
    drawXdotwithattr(e, "_hdraw_", param);
    drawXdotwithattr(e, "_tdraw_", param);
    drawXdotwithattr(e, "_hldraw_", param);
    drawXdotwithattr(e, "_tldraw_", param);
}



void drawGraph(Agraph_t * g)
{
    Agnode_t *v;
    Agedge_t *e;
    Agraph_t *s;
    int param = 0;
    for (s = agfstsubg(g); s; s = agnxtsubg(s)) {
/*		OD_SelFlag(s) = 0;
		if (OD_Selected(s) == 1)
			param = 1;
		else*/
	param = 0;
	drawXdotwithattrs(s, param);
    }

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
/*		if (OD_Selected(v) == 1)
			param = 1;
		else*/
	param = 0;
//              OD_SelFlag(v) = 0;
	drawXdotwithattr(v, "_draw_", param);	//draw primitives
	drawXdotwithattr(v, "_ldraw_", param);	//label drawing
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
/*			OD_SelFlag(e) = 0;
			if (OD_Selected(e) == 1)
				param = 1;
			else*/
	    param = 0;
	    drawXdotwithattrs(e, param);
	}
    }
    if ((view->Selection.Active > 0) && (!view->SignalBlock)) {
	view->Selection.Active = 0;
	drawGraph(g);
	view->SignalBlock = 1;
	glexpose();
	view->SignalBlock = 0;
    }

}


/*
	this function is used to cache fonts in view->fontset
*/

#if UNUSED
static void scanXdot(xdot * xDot, void *p)
{
    int id;
    sdot_op *ops = (sdot_op *) (xDot->ops);
    sdot_op *op;

    for (id = 0; id < xDot->cnt; id++) {
	op = ops + id;
	op->obj = p;
	if (op->op.kind == xd_font) {
	    add_font(view->fontset, op->op.u.font.name);	//load or set active font
	}
    }
}


static void scanXdotwithattr(void *p, char *attr)
{
    xdot *xDot;
    if ((xDot = parseXDotF(agget(p, attr), OpFns, sizeof(sdot_op)))) {
	scanXdot(xDot, p);
	freeXDot(xDot);
    }
}

static void scanXdotwithattrs(void *e)
{
    scanXdotwithattr(e, "_draw_");
    scanXdotwithattr(e, "_ldraw_");
    scanXdotwithattr(e, "_hdraw_");
    scanXdotwithattr(e, "_tdraw_");
    scanXdotwithattr(e, "_hldraw_");
    scanXdotwithattr(e, "_tldraw_");
}




/*
	iterate in nodes and edges to cache fonts, run this once or whenever a new font is added to the graph
*/


void scanGraph(Agraph_t * g)
{
    Agnode_t *v;
    Agedge_t *e;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	scanXdotwithattr(v, "_draw_");
	scanXdotwithattr(v, "_ldraw_");
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    scanXdotwithattrs(e);
	}
    }

}
#endif



#ifdef UNUSED
char *create_us_map()
{
    float x1, y1, x2, y2;
    float ox1, oy1, ox2, oy2;
    static const char filename[] = "file.txt";
    FILE *file = fopen(filename, "r");
    char line[128];		/* or other suitable maximum line size */
    int firstline = 1;
    while (fgets(line, sizeof line, file) != NULL) {	/* read a line */
	fputs(line, stdout);	/* write the line */
	sscanf(line, "%f %f %f %f", &x1, &y1, &x2, &y2);

	if (firstline) {
	    ox1 = x1;
	    oy1 = y1;
	    ox2 = x2;
	    oy2 = y2;
	    firstline = 0;
	} else if ((x1 == ox1) && (y1 == oy1) && (x2 == ox2) && (y2 == oy2)) {	/*polygon is closed here */


	}


    }
    fclose(file);
    return 0;
}
#endif
