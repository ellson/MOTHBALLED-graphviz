#ifndef GLCOMPDEFS_H
#define GLCOMPDEFS_H

#ifdef _WIN32
#include "windows.h"
#endif
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
    float fgColor[4];   /* foreground color, default white */
    float gdColor[4];   /* gradient color, default gray */
    float bgColor[4];   /* background color, default gray */
    float size;           /* size of text, default 12 */
    int shadow;         /* shadow text? default 0 */
    int gradient;       /* gradient? default 0 */
    int italic;         /* italic amount, defaul 0 */
    int bold;           /* bold text? */
    int region;         /* do we have a text region */
    float regionX;        /* lower left x */
    float regionY;        /* lower left y */
    float regionW;        /* text region w */
    float regionH;        /* text region h */
    float tIncX;        /* used for texture coords, x axis amount to move */
    float tIncY;        /* used for texture coords, y axis amount to move */
    int blockRow;       /* characters per row */
    int blockCol;       /* characters per col */
    unsigned int texId; /* texture id */
	float zdepth;	//third dimension , depth of fonts
	char* fontdesc;	//font description
} texFont_t;


typedef struct
{
	texFont_t** fonts;
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
    texFont_t* font;	//pointer to font to use
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
    texFont_t* font;	//pointer to font to use
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
    texFont_t* font;	//pointer to font to use
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
    texFont_t* font;	//pointer to font to use
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
    texFont_t* font;	//pointer to font to use
} glCompSet;

#endif
