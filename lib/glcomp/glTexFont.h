/*
    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- glTexFont.h
    Date        -- 5/30/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1
*/
#ifndef __GLTEXFONTH__
#define __GLTEXFONTH__

#ifdef _WIN32
#include <stdlib.h>
#include "glpangofont.h"	
typedef float GLfloat;
#else
#include <unistd.h>
#include <GL/gl.h>	
#endif

/* Error Codes */
#define FONT_FILE_NOT_FOUND          -13 /* file was not found */
#define FONT_BAD_IMAGE_TYPE          -14 /* color mapped image or image is not uncompressed */
#define FONT_BAD_DIMENSION           -15 /* dimension is not a power of 2 */
#define FONT_BAD_BITS                -16 /* image bits is not 8, 24 or 32 */
#define FONT_BAD_DATA                -17 /* image data could not be loaded */
#define GL_TEX_FONT_VERSION          6

#ifndef vec2_t
typedef float vec2_t[2];
#endif

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

#ifdef __cplusplus
extern "C" {
#endif
/*
	check if font is already loaded
	if loaded returns the id
	else returns -1
*/
/* int fontId(fontset_t* fontset,char* fontdesc); */

/*
	Initialize fontset as an empty set
*/

extern fontset_t* fontset_init(void);

void free_font_set(fontset_t* fontset);
void copy_font(texFont_t* targetfont,const texFont_t* sourcefont);
/*
	load font via font description
	returns the id,
	if font already exists no malloc just returns the id
*/
int add_font(fontset_t* fontset,char* fontdesc);
	
/*
=============
fontLoad

Loads up out font from the passed image file name.  
=============
*/
int fontLoad (texFont_t* font,char *name);
/*
=============
fontLoadEx
    
Same as above but it loads a custom font map with row chars per row and col
chars per column.  
=============
*/
int fontLoadEx (texFont_t* font,char *name, int row, int col);
/*
=============
fontDrawString

Draws a string at (xpos, ypos) in the applications window.
=============
*/
void fontDrawString (texFont_t*, GLfloat , GLfloat , GLfloat, char*, ...);
/*
=============
fontRegion

Sets up a font region.  Upper left corner is described by (xpos, ypos).
The region is w units wide and h units tall.  
=============
*/
void fontRegion (texFont_t* font,int xpos, int ypos, int w, int h);
/*
=============
fontSize 

Sets the font size.
=============
*/
void fontSize (texFont_t* font,GLfloat size);


void fontzdepth(texFont_t* font,float zdepth);

/*
=============
fontShadow

Draws shadowed text.
=============
*/
void fontShadow (texFont_t* font);
/*
=============
fontGradient

Draws gradient text. 
=============
*/
void fontGradient (texFont_t* font);
/*
=============
fontColor*, fontShadowColor*, fonrGradientColor*


Sets color for various operations. 
=============
*/

texFont_t* font_init(void);
void fontColor (texFont_t* font,float r, float g, float b);
void fontColorA (texFont_t* font,float r, float g, float b, float a);
void fontColorp (texFont_t* font,float *clr);
void fontColorAp (texFont_t* font,float *clr);
void fontShadowColor (texFont_t* font,float r, float g, float b);
void fontShadowColorA (texFont_t* font,float r, float g, float b, float a);
void fontShadowColorp (texFont_t* font,float *clr);
void fontShadowColorAp (texFont_t* font,float *clr);
void fontGradientColor (texFont_t* font,float r, float g, float b);
void fontGradientColorA (texFont_t* font,float r, float g, float b, float a);
void fontGradientColorp (texFont_t* font,float *clr);
void fontGradientColorAp (texFont_t* font,float *clr);
#ifdef __cplusplus
}
#endif

#endif


