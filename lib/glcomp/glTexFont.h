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

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include "glpangofont.h"	
typedef float GLfloat;
#else
#include <unistd.h>
#include <GL/gl.h>	
#endif
#include "glcompdefs.h"

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
void fontRegion (texFont_t* font,float xpos, float ypos, float w, float h);
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


