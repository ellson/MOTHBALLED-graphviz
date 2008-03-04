/*
    glTexFont Library R6

    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- glTexFont.c
    Date        -- 5/30/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1
*/
//#define FONT_LIB_DEBUG /* un-comment for debug regions */
#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"


/* just a global for our default colors */
float white[] = {1.0, 1.0, 1.0, 1.0};
float gray[] = {0.5, 0.5, 0.5, 1.0};

/* the font structure we will use in the library, all values are defaults */
texFont_t font = 
{
    {1.0, 1.0, 1.0, 1.0}, /* white text */
    {0.5, 0.5, 0.5, 1.0}, /* gray gradient */
    {0.5, 0.5, 0.5, 1.0}, /* gray shadow */
    12,                   /* size 12 font */
    0,                    /* no shadow */
    0,                    /* no gradient */
    0,                    /* italic amount */
    0,                    /* no bold */
    0,0,0,0,0,            /* text region stuff */
    0.0,0.0,              /* texture inc stuff */
    FONT_BLOCK_ROW,       /* characters per row */
    FONT_BLOCK_COL,       /* characters por col */
    0,                    /* texture id */
};

/*
    tPoints contains our map for the font image.  After fontMakeMap this array
    will contain all the information to get the proper key from the font map.
*/
vec2_t tPoints[257]; 

/*
=============
fontLoad

Load the passed image name and use it for the font.  
=============
*/
int fontLoad (char *name)
{
    glGenTextures (1, &font.texId);

	font.blockRow = FONT_BLOCK_ROW;
	font.blockCol = FONT_BLOCK_COL;

	fontMakeMap ();

    return fontLoadTGA (name, font.texId);
}
/*
=============
fontLoadEx

Loads the passed image which has 'col' characters per column and 'row' 
characters per row. 
=============
*/
int fontLoadEx (char *name, int row, int col)
{
    /* make sure we have valid dimensions */
/*	if (row * col != 256)
		return 0;*/

    glGenTextures (1, &font.texId);

	font.blockRow = row;
	font.blockCol = col;

	fontMakeMap ();

    return fontLoadTGA (name, font.texId);
}
/*
=============
fontDrawChar

Draws a character that is 'size' pixels in w and h.  
=============
*/
void fontDrawChar (char c, int x, int y, int size, int shadow)
{
	if (!font.gradient && !shadow)
        glColor4fv (font.fgColor);
    else if (!font.gradient && shadow)
        glColor4fv (font.bgColor);
    else if (font.gradient && !shadow)
        glColor4fv (font.gdColor);

    glBegin (GL_QUADS);
		glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1]);
		glVertex3i (x, y,0);

    	glTexCoord2f (tPoints[(int)c][0] + font.tIncX, tPoints[(int)c][1]);
		glVertex3i (x + size, y,0);

        if (!shadow)
            glColor4fv (font.fgColor);
        else glColor4fv (font.bgColor);

        glTexCoord2f (tPoints[(int)c][0] + font.tIncX, tPoints[(int)c][1] + font.tIncY);
		glVertex3i (x + size + font.italic, y + size,0);

        glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1] + font.tIncY);
		glVertex3i (x + font.italic, y + size,0);
	glEnd ();
}
/*
=============
fontScissorNormal

Normal scissor region for text rendering.  
=============
*/
void fontScissorNormal (int xpos, int ypos, int tabs, int carrage, int size, int len)
{
    int sy;
    int ex;
    int ey;

    ex = len * size * tabs;

    if (carrage)
	{
		sy = ypos - (size * carrage);
		ey = size * (carrage + 1);
	} else {
		sy = ypos;
		ey = size;
	}
    glScissor (xpos, sy, ex, ey);
}
/*
=============
fontScissorTextRegion

Scissor region used setup for a text region.
=============
*/
void fontScissorTextRegion (void)
{
    glScissor (font.regionX, font.regionY, font.regionW, font.regionH);
}
/*
=============
fontFgColorReset

Resets the font color.
=============
*/
void fontForeColorReset (void)
{
    fontColorCopy (white, font.fgColor);
}
/*
=============
fontBgColorReset

Resets the shadow color.
=============
*/
void fontShadowColorReset (void)
{
    fontColorCopy (gray, font.bgColor);
}
/*
=============
fontGdColorReset

Resets the gradient color.
=============
*/
void fontGradientColorReset (void)
{
    fontColorCopy (gray, font.gdColor);
}
/*
=============
fontReset

Resets the font.  Only resets variables that could possible change.
=============
*/
void fontReset (void)
{
    font.size = 12;
    font.shadow = 0;
    font.region = 0;
    font.gradient = 0;
    font.italic = 0;
    font.bold = 0;
    font.regionX = 0;
    font.regionY = 0;
    font.regionW = 0;
    font.regionH = 0;
    fontForeColorReset ();
    fontShadowColorReset ();
    fontGradientColorReset ();
}
/*
=============
fontRegion

Sets up a font region.  Only good for one fontDrawString. 
=============
*/
void fontRegion (int x, int y, int w, int h)
{
    font.region = 1;
    font.regionX = x;
    font.regionY = y - h;
    font.regionW = w;
    font.regionH = h;
}
/*
=============
fontSize

Sets the font size.
=============
*/
void fontSize (int size)
{
    font.size = size;
}
void fontzdepth(float zdepth)
{
	font.zdepth=zdepth;
}
/*
=============
fontShadow

Draws a shadow if called.
=============
*/
void fontShadow (void)
{
    font.shadow = 1;
}
/*
=============
fontGradient

Draws gradient text if called.
=============
*/
void fontGradient (void)
{
    font.gradient = 1;
}
/*
=============
fontRenderChar

Draws a character to the screen
Bold is just a hack, nothing special
=============
*/
void fontRenderChar (char c, int x, int y, int size)
{
    if (font.shadow)
    {
        if (!font.bold)
	        fontDrawChar (c, x + 1,  y + 1, size, 1);
        else fontDrawChar (c, x + 2,  y + 1, size, 1);
    }
    


	fontDrawChar (c, x, y, size, 0);

	if (font.bold)
        fontDrawChar (c, x + 1, y, size, 0);
}
/*
=============
fontSlashParser

Handles all the fun that comes with a \\, returns amount to advance string.
After this funtion *buffptr ++ will be the next character to draw or parse.
=============
*/
int fontSlashParser (char *buffPtr, int *x, int *y)
{
    int ret = 0;

    *buffPtr ++;

    if (!*buffPtr)
        return ret;

    switch (*buffPtr)
    {
        case 'a':
        case 'c':
            *x -= font.size;
            return fontSetColorFromToken (buffPtr);
        break;
        case 'i':
            *x -= font.size;
            return fontItalicsMode (buffPtr);
        break;
        case 'b':
            *x -= font.size;
            return fontBoldMode (buffPtr);
        break;
        default:
            *buffPtr --;
            fontRenderChar (*buffPtr, *x, *y, font.size);
            return ret;
        break;
    }
}
/*
=============
fontWalkString

Does the actual rendering of our string.  
=============
*/
void fontWalkString (char *buffPtr, int xpos, int ypos, int *vPort,float width)
{
    int size = font.size;
	int x = xpos;
	int y = ypos;
	int carrage = 0;
	int tabs = 0;
    int len = strlen (buffPtr);
    int xMax;
	int charCount;
	int maxcharCount;
	char* tempC;
	int charGap;
	xMax = vPort[0] + vPort[2];

    carrage = fontGetCharHits (buffPtr, '\n');
	tabs = fontGetCharHits (buffPtr, '\t');

	if (!tabs)
		tabs = 1;
	else tabs *= FONT_TAB_SPACE;

/*    if (font.region)
    {
        fontScissorTextRegion ();
        x = font.regionX;
        y = (font.regionY + font.regionH) - font.size;
    } else 
	fontScissorNormal (xpos, ypos, tabs, carrage, font.size, len); */

#ifdef FONT_LIB_DEBUG
    glClearColor (1,0,1,1);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif

    /* lets draw! */
	//width for each char should be calculated , we need the max line length of the paragraph

	tempC=buffPtr;
	charCount=0;
	maxcharCount=0;

	while (*tempC != '\0')
	{
		if ( *tempC == '\n')
		{
			if (charCount > maxcharCount)
					maxcharCount=charCount;
			charCount=0;

		}
		else
		{
			if ( *tempC == '\t')
				charCount=charCount+FONT_TAB_SPACE;
			else
				charCount++;
		}
		tempC++;
	}
	if (charCount > maxcharCount)
		maxcharCount=charCount;
	charGap=(int)(width / (float)maxcharCount);


	for ( ; *buffPtr; *buffPtr ++, x += charGap) //size*0.7 is the distance between2 characters
	{
	//	if (x > xMax)
	//		break;
    /*    if (font.region)
        {
            if (x + size > (font.regionX + font.regionW))
            {
                y -= size;
                x = font.regionX;
            }
            if (y < font.regionY)
                break;
        }*/




		if(*buffPtr==' ')		//SPACE
			x = x - size;

		switch (*buffPtr)
		{

		
			case '\n':
				y -= size;
                x = xpos - size;
				continue; 
			break;
			case '\t':
				x += (size * FONT_TAB_SPACE);
				continue; 
			break;
            case '\\':
                buffPtr += fontSlashParser (buffPtr, &x, &y);
                if (*buffPtr == '\n' || *buffPtr == '\t')
                {
                    buffPtr -= 1;
                    continue;
                }
            break;
            default :
                fontRenderChar (*buffPtr, x, y, size);
            break;
		}
	}
}
/*
=============
fontDrawString

Renders a string at xpos, ypos.
=============
*/
void fontDrawString (int xpos, int ypos, char *s,int width,...)
{
	va_list	msg;
    char buffer[FONT_MAX_LEN] = {'\0'};
    int vPort[4];
	
	va_start (msg, s);
#ifdef _WIN32
	_vsntprintf (buffer, FONT_MAX_LEN - 1, s, msg);	
#else /* linux */
	vsnprintf (buffer, FONT_MAX_LEN - 1, s, msg);	
#endif
	va_end (msg);

    /* get current viewport */
    glGetIntegerv (GL_VIEWPORT, vPort);
    /* setup various opengl things that we need */

	fontSetModes (FONT_GET_MODES);
	//if constant location is desired for fonts turn on these comment outs    
/*	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();

  
	glOrtho (0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();*/
	glBindTexture (GL_TEXTURE_2D, font.texId);

    /* draw the string */


	fontWalkString (buffer, xpos, ypos, vPort,(float)width);
	
/*	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();*/

	fontSetModes (FONT_RESTORE_MODES);
    fontReset ();
}
/*
=============
fontSetColorFromToken

Grabs a color token from a buffer and sets color.  
=============
*/	
int fontSetColorFromToken (char *s)
{
	int clr[4];
	int ret = 1;

	if (*s == 'c')
	{
		s += 1;
		if (sscanf (s, "(%d %d %d)", &clr[0], &clr[1], &clr[2]) != 3)
			return -1;
		fontColor (clr[0] * FONT_ITOF, clr[1] * FONT_ITOF, clr[2] * FONT_ITOF);
	} else if (*s == 'a')
	{
		s += 1;
		if (sscanf (s, "(%d %d %d %d)", &clr[0], &clr[1], &clr[2], &clr[3]) != 4)
			return -1;
		fontColorA (clr[0] * FONT_ITOF, clr[1] * FONT_ITOF, clr[2] * FONT_ITOF, clr[3] * FONT_ITOF);
	}

	while (*s != ')' && ret ++)
		s += 1;

    return ret + 1;
}
/*
=============
fontItalicsMode

Either turns on or off italics.
=============
*/	
int fontItalicsMode (char *s)
{
    s += 1;

    if (*s == '+')
        font.italic = FONT_ITALIC;
    else if (*s == '-')
        font.italic = 0;
    else return -1;

    return 2;
}
/*
=============
fontBoldMode

Either turns on or off bold.
=============
*/	
int fontBoldMode (char *s)
{
    s += 1;

    if (*s == '+')
        font.bold = 1;
    else if (*s == '-')
        font.bold = 0;
    else return -1;

    return 2;
}
/*
=============
fontGetCharHits

Returns number of times a character is found in a buffer.
=============
*/
int fontGetCharHits (char *s, char f)
{
	int hits = 0;

	while (*s)
	{
		if (*s == f)
			hits ++;
		*s ++;
	}

	return hits;
}
/*
=============
fontMakeMap

Makes the font map which allows the correct characters to be drawn.
=============
*/
void fontMakeMap (void)
{
#define VCOPY(d,x,y) {d[0] = x; d[1] = y;}
	int i = 0;
	float x, y;


	font.tIncX = (float)pow (font.blockCol, -1);
	font.tIncY = (float)pow (font.blockRow, -1);

	
	for (y = 1 - font.tIncY; y >= 0; y -= font.tIncY)
		for (x = 0; x <= 1 - font.tIncX; x += font.tIncX, i ++)
			VCOPY (tPoints[i], x, y);
#undef VCOPY
}
/*
=============
fontSetModes

Sets or restores OpenGL modes that we need to use.  Here to prevent the drawing
of text from messing up other stuff.  
=============
*/
void fontSetModes (int state)
{
	static int matrixMode;
	static int polyMode[2];
	static int lightingOn;
	static int blendOn;
	static int depthOn;
	static int textureOn;
	static int scissorOn;
	static int blendSrc;
	static int blendDst;

	/* grab the modes that we might need to change */
	if (state == FONT_GET_MODES)
	{
		glGetIntegerv(GL_POLYGON_MODE, polyMode);

		if (polyMode[0] != GL_FILL)
			glPolygonMode (GL_FRONT, GL_FILL);
		if (polyMode[1] != GL_FILL)
			glPolygonMode (GL_BACK, GL_FILL);
	
		textureOn = glIsEnabled (GL_TEXTURE_2D);
		if (!textureOn)
			glEnable (GL_TEXTURE_2D);

		depthOn = glIsEnabled (GL_DEPTH_TEST);

		if (depthOn)
			glDisable (GL_DEPTH_TEST);

		lightingOn= glIsEnabled (GL_LIGHTING);        
	
		if (lightingOn) 
			glDisable(GL_LIGHTING);

		scissorOn= glIsEnabled (GL_SCISSOR_TEST);        
	
/*		if (!scissorOn) 
			glEnable (GL_SCISSOR_TEST);*/

		glGetIntegerv(GL_MATRIX_MODE, &matrixMode); 
	
		/* i don't know if this is correct */
		blendOn= glIsEnabled (GL_BLEND);        
        glGetIntegerv (GL_BLEND_SRC, &blendSrc);
	    glGetIntegerv (GL_BLEND_DST, &blendDst);
		if (!blendOn)
			glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else if (state == FONT_RESTORE_MODES)
	{
		/* put everything back where it was before */
		if (polyMode[0] != GL_FILL)
			glPolygonMode (GL_FRONT, polyMode[0]);
		if (polyMode[1] != GL_FILL)
			glPolygonMode (GL_BACK, polyMode[1]);
	
		if (lightingOn)
			glEnable(GL_LIGHTING);
	
		/* i don't know if this is correct */
		if (!blendOn)
        {
			glDisable (GL_BLEND);
            glBlendFunc (blendSrc, blendDst);
        } else glBlendFunc (blendSrc, blendDst);

		if (depthOn)
			glEnable (GL_DEPTH_TEST);

		if (!textureOn)
			glDisable (GL_TEXTURE_2D);
		
		if (!scissorOn) 
			glDisable (GL_SCISSOR_TEST);
	
		glMatrixMode (matrixMode);
	}
}
