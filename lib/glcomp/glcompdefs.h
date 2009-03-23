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
#ifndef GLCOMPDEFS_H
#define GLCOMPDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#endif
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef WIN32
#define strdup _strdup
#endif


#define	GLCOMPSET_PANEL_COLOR_R		(GLfloat)0.16
#define	GLCOMPSET_PANEL_COLOR_G		(GLfloat)0.44
#define	GLCOMPSET_PANEL_COLOR_B		(GLfloat)0.87
#define	GLCOMPSET_PANEL_COLOR_ALPHA	(GLfloat)0.5
#define	GLCOMPSET_PANEL_SHADOW_COLOR_R		(GLfloat)0
#define	GLCOMPSET_PANEL_SHADOW_COLOR_G		(GLfloat)0
#define	GLCOMPSET_PANEL_SHADOW_COLOR_B		(GLfloat)0
#define	GLCOMPSET_PANEL_SHADOW_COLOR_A		(GLfloat)0.3
#define GLCOMPSET_PANEL_SHADOW_WIDTH		(GLfloat)4

#define	GLCOMPSET_BUTTON_COLOR_R		(GLfloat)0
#define	GLCOMPSET_BUTTON_COLOR_G		(GLfloat)1
#define	GLCOMPSET_BUTTON_COLOR_B		(GLfloat)0.3
#define	GLCOMPSET_BUTTON_COLOR_ALPHA	(GLfloat)0.6
#define	GLCOMPSET_BUTTON_THICKNESS		(GLfloat)3
#define	GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS		(GLfloat)1.7
#define GLCOMPSET_FONT_SIZE				(GLfloat)14
#define	GLCOMPSET_BUTTON_FONT_COLOR_R		(GLfloat)1
#define	GLCOMPSET_BUTTON_FONT_COLOR_G		(GLfloat)1
#define	GLCOMPSET_BUTTON_FONT_COLOR_B		(GLfloat)1
#define	GLCOMPSET_BUTTON_FONT_COLOR_ALPHA	(GLfloat)1
#define GLCOMPSET_FONT_SIZE_FACTOR			(GLfloat)0.7
#define	GLCOMPSET_LABEL_COLOR_R		(GLfloat)0
#define	GLCOMPSET_LABEL_COLOR_G		(GLfloat)0
#define	GLCOMPSET_LABEL_COLOR_B		(GLfloat)0
#define	GLCOMPSET_LABEL_COLOR_ALPHA	(GLfloat)1

#define GLCOMPSET_PANEL_BEVEL				(GLfloat)0.1
#define	GLCOMPSET_BEVEL_DIFF				(GLfloat)0.001
#define GLCOMPSET_DEFAULT_PAD		(GLfloat)3

#define FONT_MAX_LEN                1024 /* maximum chars to draw to the screen, used for buffers also */
#define FONT_TAB_SPACE              4    /* spaces to draw for a tab, make option? */

#define C_DPI              16
#define R_DPI              16


typedef void (*callbackfunc_t) (void *component);
typedef enum { inverted_y,scientific_y} glCompOrientation;

typedef struct _glCompPoint {
    GLfloat x, y;
} glCompPoint;
typedef struct {
    GLfloat R;
    GLfloat G;
    GLfloat B;
    GLfloat A;			//Alpha
} glCompColor;
typedef struct {
    unsigned int id;
    float w, h;
} glCompTexture;

typedef struct 
{
	int matrix;
	int poly[2];
	int islightingon;
	int isblendon;
	int isdepthon;
	int istextureon;
	int blendfrom;
	int blendto;
}fontglcache;



typedef struct
{
	char* fontdesc;	//font description
	glCompColor color;
    float fontheight;           /* size of text, default 12 */
    float tIncX;        /* used for texture coords, x axis amount to move */
    float tIncY;        /* used for texture coords, y axis amount to move */
    int blockRow;       /* characters per row */
    int blockCol;       /* characters per col */
    int texId; /* texture id */
	float zdepth;	//third dimension , depth of fonts
	float  bmp[257][2]; //texture bitmaps
	fontglcache glcache;
} glCompText;


typedef struct
{
	glCompText** fonts;
	int count;
	int activefont;
	char* font_directory;	//location where the glfont files are stored
}fontset_t;






typedef struct _glCompPanel {
    glCompPoint pos;
    GLfloat width, height;
    GLfloat shadowwidth;
    GLfloat bevel;
    glCompColor color;
    glCompColor shadowcolor;
    int enabled;
    int visible;
    void *parentset;		//parent compset
    int data;
    glCompText* font;	//pointer to font to use
	glCompOrientation orientation;

} glCompPanel;
typedef struct _glCompTrackBar {
	GLfloat width,height;
	glCompPanel* outerpanel;
	glCompPanel* trackline;
	glCompPanel* indicator;

    GLfloat bevel;
    glCompColor color;
    glCompColor shadowcolor;


	float value;
	float maxvalue;
	float minvalue;
	int enabled;
    int visible;
    void *parentset;		//parent compset
    int data;
    glCompText* font;	//pointer to font to use
	glCompOrientation orientation;

}glCompTrackBar;
typedef struct _glCompLabel {
    glCompPoint pos;
    GLfloat size;
    GLfloat bevel;
    glCompColor color;
    int visible;
    char *text;
    GLfloat fontsizefactor;
    glCompPanel *panel;		//container panel
    glCompText* font;	//pointer to font to use
	glCompOrientation orientation;

} glCompLabel;


typedef struct _glCompButton {
    glCompPoint pos;
    GLfloat width, height;
    GLfloat bevel;
    GLfloat thickness;
    glCompColor color;
    glCompColor fontcolor;
    char *caption;
    int enabled;
    int visible;
    int status;			//0 not pressed 1 pressed;
    int groupid;
    void *parentset;		//parent compset
    GLfloat fontsize;
    int hasglyph;
    glCompTexture *glyph;
    int glyphwidth, glyphheight;
    glCompPanel *panel;		//container panel
    callbackfunc_t callbackfunc;	//call back for button click
    void *customptr;		//general purpose void pointer to pass to call back
    int data;
    glCompText* font;	//pointer to font to use
	glCompOrientation orientation;

} glCompButton;

typedef struct {
    glCompPanel **panels;
    glCompButton **buttons;
    glCompLabel **labels;

    int panelcount;
    int buttoncount;
    int labelcount;
    int active;			//0 dont draw, 1 draw
    int enabled;		//0 disabled 1 enabled(allow mouse interaction)
    GLfloat clickedX, clickedY;
    glCompText* font;	//pointer to font to use
} glCompSet;

#endif
