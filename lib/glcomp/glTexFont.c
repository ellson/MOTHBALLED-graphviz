/* vim:set shiftwidth=4 ts=8: */
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
#include "glpangofont.h"
#include "memory.h"

/* just a global for our default colors */
float white[] = {1.0, 1.0, 1.0, 1.0};
float gray[] = {0.5, 0.5, 0.5, 1.0};


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
int fontLoad (texFont_t* font,char *name)
{
    glGenTextures (1, &(font->texId));

	font->blockRow = FONT_BLOCK_ROW;
	font->blockCol = FONT_BLOCK_COL;

	fontMakeMap (font);

//    return fontLoadTGA (name, font.texId);
    return fontLoadPNG ("c:\\pango_test.png", font->texId);

}
/*
=============
fontLoadEx

Loads the passed image which has 'col' characters per column and 'row' 
characters per row. 
=============
*/
int fontLoadEx (texFont_t* font,char *name, int row, int col)
{
    /* make sure we have valid dimensions */
/*	if (row * col != 256)
		return 0;*/

    glGenTextures (1, &(font->texId));

	font->blockRow = row;
	font->blockCol = col;

	fontMakeMap (font);

    return fontLoadTGA (name, font->texId);
}
/*
=============
fontDrawChar

Draws a character that is 'size' pixels in w and h.  
=============
*/
void fontDrawChar (texFont_t* font,char c, GLfloat x, GLfloat y, GLfloat size, int shadow)
{
	if (!font->gradient && !shadow)
        glColor4fv (font->fgColor);
    else if (!font->gradient && shadow)
        glColor4fv (font->bgColor);
    else if (font->gradient && !shadow)
        glColor4fv (font->gdColor);

    glBegin (GL_QUADS);
		glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1]);
		glVertex3f (x, y,0);

    	glTexCoord2f (tPoints[(int)c][0] + font->tIncX, tPoints[(int)c][1]);
		glVertex3f (x + size, y,0);

        if (!shadow)
            glColor4fv (font->fgColor);
        else glColor4fv (font->bgColor);

        glTexCoord2f (tPoints[(int)c][0] + font->tIncX, tPoints[(int)c][1] + font->tIncY);
		glVertex3f (x + size + font->italic, y + size,0);

        glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1] + font->tIncY);
		glVertex3f (x + font->italic, y + size,0);
	glEnd ();
}
/*
=============
fontScissorNormal

Normal scissor region for text rendering.  
=============
*/
void fontScissorNormal (texFont_t* font,int xpos, int ypos, int tabs, int carrage, int size, int len)
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
void fontScissorTextRegion (texFont_t* font)
{
    glScissor (font->regionX, font->regionY, font->regionW, font->regionH);
}
/*
=============
fontFgColorReset

Resets the font color.
=============
*/
void fontForeColorReset (texFont_t* font)
{
    fontColorCopy (white, font->fgColor);
}
/*
=============
fontBgColorReset

Resets the shadow color.
=============
*/
void fontShadowColorReset (texFont_t* font)
{
    fontColorCopy (gray, font->bgColor);
}
/*
=============
fontGdColorReset

Resets the gradient color.
=============
*/
void fontGradientColorReset (texFont_t* font)
{
    fontColorCopy (gray, font->gdColor);
}
/*
=============
fontReset

Resets the font.  Only resets variables that could possible change.
=============
*/
void fontReset (texFont_t* font)
{
    font->size = 12;
    font->shadow = 0;
    font->region = 0;
    font->gradient = 0;
    font->italic = 0;
    font->bold = 0;
    font->regionX = 0;
    font->regionY = 0;
    font->regionW = 0;
    font->regionH = 0;
    fontForeColorReset (font);
    fontShadowColorReset (font);
    fontGradientColorReset (font);
}
/*
=============
fontRegion

Sets up a font region.  Only good for one fontDrawString. 
=============
*/
void fontRegion (texFont_t* font,int x, int y, int w, int h)
{
    font->region = 1;
    font->regionX = x;
    font->regionY = y - h;
    font->regionW = w;
    font->regionH = h;
}
/*
=============
fontSize

Sets the font size.
=============
*/
void fontSize (texFont_t* font,GLfloat size)
{
    font->size = size;
}
void fontzdepth(texFont_t* font,GLfloat zdepth)
{
	font->zdepth=zdepth;
}
/*
=============
fontShadow

Draws a shadow if called.
=============
*/
void fontShadow (texFont_t* font)
{
    font->shadow = 1;
}
/*
=============
fontGradient

Draws gradient text if called.
=============
*/
void fontGradient (texFont_t* font)
{
    font->gradient = 1;
}
/*
=============
fontRenderChar

Draws a character to the screen
Bold is just a hack, nothing special
=============
*/
void fontRenderChar (texFont_t* font,char c, GLfloat x, GLfloat y, GLfloat size)
{
    if (font->shadow)
    {
        if (!font->bold)
	        fontDrawChar (font,c, x + 1.0,  y + 1.0, size, 1);
        else fontDrawChar (font,c, x + 2.0,  y + 1.0, size, 1);
    }
    


	fontDrawChar (font,c, x, y, size, 0);

	if (font->bold)
        fontDrawChar (font,c, x + 1.0, y, size, 0);
}
/*
=============
fontSlashParser

Handles all the fun that comes with a \\, returns amount to advance string.
After this funtion *buffptr ++ will be the next character to draw or parse.
=============
*/
int fontSlashParser (texFont_t* font,char *buffPtr, GLfloat *x, GLfloat *y)
{
    int ret = 0;

    *buffPtr ++;

    if (!*buffPtr)
        return ret;

    switch (*buffPtr)
    {
        case 'a':
        case 'c':
            *x -= font->size;
            return fontSetColorFromToken (font,buffPtr);
        break;
        case 'i':
            *x -= font->size;
            return fontItalicsMode (font,buffPtr);
        break;
        case 'b':
            *x -= font->size;
            return fontBoldMode (font,buffPtr);
        break;
        default:
            *buffPtr --;
            fontRenderChar (font,*buffPtr, *x, *y, font->size);
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
void fontWalkString (texFont_t* font,char *buffPtr, GLfloat xpos, GLfloat ypos, int *vPort,float width)
{
    GLfloat size = font->size;
	GLfloat x = xpos;
	GLfloat y = ypos;
	GLfloat carrage = 0;
	GLfloat tabs = 0;
    int xMax;
	int charCount;
	int maxcharCount;
	char* tempC;
	GLfloat charGap;
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
	charGap=(width / (float)maxcharCount);


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



/*
		if(*buffPtr==' ')		//SPACE
			x = x + size;*/

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
                buffPtr += fontSlashParser (font,buffPtr, &x, &y);
                if (*buffPtr == '\n' || *buffPtr == '\t')
                {
                    buffPtr -= 1;
                    continue;
                }
            break;
            default :
                fontRenderChar (font,*buffPtr, x, y, size);
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
void fontDrawString (texFont_t* font,GLfloat xpos, GLfloat ypos, char *s,GLfloat width,...)
{
	va_list	msg;
    char buffer[FONT_MAX_LEN] = {'\0'};
    int vPort[4];
	
	va_start (msg, s);
#ifdef _WIN32
	_vsntprintf (buffer, FONT_MAX_LEN - 1, s, msg);	
#else /* not Windows */
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
	glBindTexture (GL_TEXTURE_2D, font->texId);

    /* draw the string */


	fontWalkString (font,buffer, xpos, ypos, vPort,(float)width);
	
/*	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();*/

	fontSetModes (FONT_RESTORE_MODES);
    fontReset (font);
}
/*
=============
fontSetColorFromToken

Grabs a color token from a buffer and sets color.  
=============
*/	
int fontSetColorFromToken (texFont_t* font,char *s)
{
	int clr[4];
	int ret = 1;

	if (*s == 'c')
	{
		s += 1;
		if (sscanf (s, "(%d %d %d)", &clr[0], &clr[1], &clr[2]) != 3)
			return -1;
		fontColor (font,clr[0] * FONT_ITOF, clr[1] * FONT_ITOF, clr[2] * FONT_ITOF);
	} else if (*s == 'a')
	{
		s += 1;
		if (sscanf (s, "(%d %d %d %d)", &clr[0], &clr[1], &clr[2], &clr[3]) != 4)
			return -1;
		fontColorA (font,clr[0] * FONT_ITOF, clr[1] * FONT_ITOF, clr[2] * FONT_ITOF, clr[3] * FONT_ITOF);
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
int fontItalicsMode (texFont_t* font,char *s)
{
    s += 1;

    if (*s == '+')
        font->italic = FONT_ITALIC;
    else if (*s == '-')
        font->italic = 0;
    else return -1;

    return 2;
}
/*
=============
fontBoldMode

Either turns on or off bold.
=============
*/	
int fontBoldMode (texFont_t* font,char *s)
{
    s += 1;

    if (*s == '+')
        font->bold = 1;
    else if (*s == '-')
        font->bold = 0;
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
void fontMakeMap (texFont_t* font)
{
#define VCOPY(d,x,y) {d[0] = x; d[1] = y;}
	int i = 0;
	float x, y;


	font->tIncX = (float)pow (font->blockCol, -1);
	font->tIncY = (float)pow (font->blockRow, -1);

	
	for (y = 1 - font->tIncY; y >= 0; y -= font->tIncY)
		for (x = 0; x <= 1 - font->tIncX; x += font->tIncX, i ++)
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


static int fontId(fontset_t* fontset,char* fontdesc)
{
	int ind=0;
	for (ind=0;ind < fontset->count;ind ++)
	{
		if (strcmp(fontset->fonts[ind]->fontdesc,fontdesc) == 0)
			return ind;
	}
	return -1;
}
/*
	load font via font description
	returns the id,
	if font already exists no malloc just returns the id
*/

texFont_t* font_init()
{
    texFont_t* font = NEW(texFont_t);

	font->fgColor[0]=1.0;font->fgColor[1]=1.0;font->fgColor[2]=1.0;font->fgColor[3]=1.0;
	font->gdColor[0]=0.5;font->gdColor[1]=0.5;font->gdColor[2]=0.5;font->gdColor[3]=1.0;
	font->bgColor[0]=0.5;font->bgColor[1]=0.5;font->bgColor[2]=0.5;font->bgColor[3]=1.0;
	font->size=12;
	font->shadow=0;
	font->gradient=0;
	font->italic=0;
	font->bold=0;
	font->region=0;
	font->regionX=0;
	font->regionY=0;
	font->regionW=0;
	font->regionH=0;
	font->tIncX=0.0;
	font->tIncY=0.0;
	font->blockRow=FONT_BLOCK_ROW;
	font->blockCol=FONT_BLOCK_COL;
	font->texId=-1;
	font->fontdesc=(char*)0;

	fontMakeMap (font);

    return font;
}

void copy_font(texFont_t* targetfont,const texFont_t* sourcefont)
{
	targetfont->fgColor[0]=sourcefont->fgColor[0];
	targetfont->fgColor[1]=sourcefont->fgColor[1];
	targetfont->fgColor[2]=sourcefont->fgColor[2];
	targetfont->fgColor[3]=sourcefont->fgColor[3];
	targetfont->gdColor[0]=sourcefont->gdColor[0];
	targetfont->gdColor[1]=sourcefont->gdColor[1];
	targetfont->gdColor[2]=sourcefont->gdColor[2];
	targetfont->gdColor[3]=sourcefont->gdColor[3];
	targetfont->bgColor[0]=sourcefont->bgColor[0];
	targetfont->bgColor[1]=sourcefont->bgColor[1];
	targetfont->bgColor[2]=sourcefont->bgColor[2];
	targetfont->bgColor[3]=sourcefont->bgColor[3];
	targetfont->size=sourcefont->size;
	targetfont->shadow=sourcefont->shadow;
	targetfont->gradient=sourcefont->gradient;
	targetfont->italic=sourcefont->italic;
	targetfont->bold=sourcefont->bold;
	targetfont->region=sourcefont->region;
	targetfont->regionX=sourcefont->regionX;
	targetfont->regionY=sourcefont->regionY;
	targetfont->regionW=sourcefont->regionW;
	targetfont->regionH=sourcefont->regionH;
	targetfont->tIncX=sourcefont->tIncX;
	targetfont->tIncY=sourcefont->tIncY;
	targetfont->blockRow=sourcefont->blockRow;
	targetfont->blockCol=sourcefont->blockCol;
	targetfont->texId=sourcefont->texId;
	if (targetfont->fontdesc)
		free(targetfont->fontdesc);
	if(sourcefont->fontdesc!=(char*)0)
		targetfont->fontdesc=strdup(sourcefont->fontdesc);
	else
		targetfont->fontdesc=(char*)0;
	fontMakeMap (targetfont);
}

#ifndef _WIN32
#define TMPTEMP "/tmp/_sfXXXX"
#endif

fontset_t* fontset_init()
{
    fontset_t* fs = NEW(fontset_t);
	fs->activefont=-1;
	fs->count=0;
#ifdef _WIN32
	fs->font_directory = "c:/fontfiles"; //FIX ME
#else
    fs->font_directory = strdup (TMPTEMP);
    mkdtemp (fs->font_directory);
#endif
	fs->fonts=0;
    return fs;
}

static char* fontpath = NULL;
static int fontpathsz = 0;

int add_font(fontset_t* fontset,char* fontdesc)
{
    int id;	
    int sz;
    texFont_t* tf;

    id=fontId(fontset,fontdesc);

    if (id==-1) {
	sz = strlen(fontset->font_directory)+strlen(fontdesc)+6;
	if (sz > fontpathsz) {
	    fontpathsz = 2*sz;
	    fontpath = ALLOC (fontpathsz, fontpath, char); 
        }
	sprintf(fontpath,"%s/%s.png",fontset->font_directory,fontdesc);
	if(create_font_file(fontdesc,fontpath,(float)32,(float)32)==0) {
	    fontset->fonts = ALLOC(fontset->count+1,fontset->fonts,texFont_t*);
	    fontset->fonts[fontset->count] = tf = font_init ();
	    tf->fontdesc = strdup(fontdesc);
	    glGenTextures (1, &(tf->texId));	//get  opengl texture name
	    if ((tf->texId >= 0) && fontLoadPNG (fontpath, tf->texId)) {
		fontset->activefont=fontset->count;
		fontset->count++;
		return fontset->count;
	    }
	    else
		return -1;
	}
	else
	    return -1;
    }
    else
	return id;
}

void free_font_set(fontset_t* fontset)
{
    int ind;
    for (ind=0;ind < fontset->count;ind ++) {
#ifndef _WIN32
	sprintf(fontpath,"%s/%s.png",fontset->font_directory,fontset->fonts[ind]->fontdesc);
	unlink (fontpath);
#endif
	free(fontset->fonts[ind]->fontdesc);
	free(fontset->fonts[ind]);
    }
    free(fontset->fonts);
#ifndef _WIN32
    if (fontset->font_directory)
	rmdir (fontset->font_directory);
#endif
	if (fontset->font_directory)
		free(fontset->font_directory);
    free(fontset);
}

