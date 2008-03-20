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
    int size;           /* size of text, default 12 */
    int shadow;         /* shadow text? default 0 */
    int gradient;       /* gradient? default 0 */
    int italic;         /* italic amount, defaul 0 */
    int bold;           /* bold text? */
    int region;         /* do we have a text region */
    int regionX;        /* lower left x */
    int regionY;        /* lower left y */
    int regionW;        /* text region w */
    int regionH;        /* text region h */
    float tIncX;        /* used for texture coords, x axis amount to move */
    float tIncY;        /* used for texture coords, y axis amount to move */
    int blockRow;       /* characters per row */
    int blockCol;       /* characters per col */
    unsigned int texId; /* texture id */
	float zdepth;	//third dimension , depth of fonts
} texFont_t;

#endif
#ifdef __cplusplus
extern "C" {
#endif

/*
=============
fontLoad

Loads up out font from the passed image file name.  
=============
*/
int fontLoad (char *name);
/*
=============
fontLoadEx
    
Same as above but it loads a custom font map with row chars per row and col
chars per column.  
=============
*/
int fontLoadEx (char *name, int row, int col);
/*
=============
fontDrawString

Draws a string at (xpos, ypos) in the applications window.
=============
*/
void fontDrawString (int xpos, int ypos, char *s,int width,...);
/*
=============
fontRegion

Sets up a font region.  Upper left corner is described by (xpos, ypos).
The region is w units wide and h units tall.  
=============
*/
void fontRegion (int xpos, int ypos, int w, int h);
/*
=============
fontSize 

Sets the font size.
=============
*/
void fontSize (int size);


void fontzdepth(float zdepth);

/*
=============
fontShadow

Draws shadowed text.
=============
*/
void fontShadow (void);
/*
=============
fontGradient

Draws gradient text. 
=============
*/
void fontGradient (void);
/*
=============
fontColor*, fontShadowColor*, fonrGradientColor*

Sets color for various operations. 
=============
*/
void fontColor (float r, float g, float b);
void fontColorA (float r, float g, float b, float a);
void fontColorp (float *clr);
void fontColorAp (float *clr);
void fontShadowColor (float r, float g, float b);
void fontShadowColorA (float r, float g, float b, float a);
void fontShadowColorp (float *clr);
void fontShadowColorAp (float *clr);
void fontGradientColor (float r, float g, float b);
void fontGradientColorA (float r, float g, float b, float a);
void fontGradientColorp (float *clr);
void fontGradientColorAp (float *clr);
#ifdef __cplusplus
}
#endif
