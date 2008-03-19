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

/*Open GL basic component set
  includes glPanel,glCompButton,glCompCustomButton,clCompLabel,glCompStyle
*/
#ifndef GLCOMPSET_H
#define GLCOMPSET_H

#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"
//#include "gltemplate.h"
#include "glcomptexture.h"

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

typedef void (*callbackfunc_t) (void *component);

typedef struct _glCompPoint {
    GLfloat x, y;
} glCompPoint;
typedef struct {
    GLfloat R;
    GLfloat G;
    GLfloat B;
    GLfloat A;			//Alpha
} glCompColor;

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


} glCompPanel;
typedef struct _glCompLabel {
    glCompPoint pos;
    GLfloat size;
    GLfloat bevel;
    glCompColor color;
    int visible;
    char *text;
    GLfloat fontsizefactor;
    glCompPanel *panel;		//container panel
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
} glCompSet;

glCompPanel *glCompPanelNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h);
void glCompSetClear(glCompSet * s);
int glCompSetAddPanel(glCompSet * s, glCompPanel * p);
int glCompSetRemovePanel(glCompSet * s, glCompPanel * p);
int glCompSetDraw(glCompSet * s);

glCompButton *glCompButtonNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,
			      char *caption, char *glyphfile,
			      int glyphwidth, int glyphheight);
int glCompSetAddButton(glCompSet * s, glCompButton * p);
int glCompSetRemoveButton(glCompSet * s, glCompButton * p);
int glCompDrawButton(glCompButton * p);


int glCompSetDraw(glCompSet * s);
int glCompSetHide(glCompSet * s);
int glCompSetShow(glCompSet * s);
void glCompDrawBegin();		//pushes a gl stack 
void glCompDrawEnd();		//pops the gl stack 
int glCompDrawPanel(glCompPanel * p);
int glCompSetClick(glCompSet * s, int x, int y);
int glCompSetRelease(glCompSet * s, int x, int y);
void glCompSetGetPos(int x, int y, float *X, float *Y, float *Z);
int glCompPointInButton(glCompButton * p, float x, float y);
void glCompButtonClick(glCompButton * p);

glCompLabel *glCompLabelNew(GLfloat x, GLfloat y, GLfloat size,
			    char *text);
int glCompSetAddLabel(glCompSet * s, glCompLabel * p);
int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p);
int glCompDrawLabel(glCompLabel * p);
int glCompLabelSetText(glCompLabel * p, char *text);
int glCompPanelHide(glCompPanel * p);
int glCompPanelShow(glCompPanel * p);
int glCompSetDrawPanels(glCompSet * s);


#endif
