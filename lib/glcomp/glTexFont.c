#include "glTexFont.h"
#include "glpangofont.h"
#include "memory.h"

vec2_t tPoints[257]; 


int glCompLoadFont (glCompText* font,char *name)
{
    glGenTextures (1, &(font->texId));
	fontMakeMap (font);
    return glCompLoadFontPNG ("c:\\pango_test.png", font->texId);

}


void fontForeColorReset (glCompText* font)
{

	font->color.R=1.00;
	font->color.G=1.00;
	font->color.B=1.00;
	font->color.A=1.00;


}
void fontReset (glCompText* font)
{
    font->fontheight = 12;
    fontForeColorReset (font);
}
void fontSize (glCompText* font,GLfloat size)
{
    font->fontheight = size;
}
void fontzdepth(glCompText* font,GLfloat zdepth)
{
	font->zdepth=zdepth;
}
void fontWalkString (glCompText* font,char *bf, GLfloat xpos, GLfloat ypos, int *vPort,float width)
{
	GLfloat size = font->fontheight;
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

    carrage = (GLfloat) fontGetCharHits (bf, '\n');
	tabs = (GLfloat) fontGetCharHits (bf, '\t');

	if (!tabs)
		tabs = 1;
	else tabs *= FONT_TAB_SPACE;
    /* lets draw! */
	//width for each char should be calculated , we need the max line length of the paragraph

	tempC=bf;
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


	for ( ; *bf; *bf ++, x += charGap) //size*0.7 is the distance between2 characters
	{

	    glBegin (GL_QUADS);
		glTexCoord2f (tPoints[(int)(*bf)][0], tPoints[(int)(*bf)][1]);
		glVertex3f (x, y,0);

    	glTexCoord2f (tPoints[(int)(*bf)][0] + font->tIncX, tPoints[(int)(*bf)][1]);
		glVertex3f (x + size, y,0);

		glColor4f (font->color.R,font->color.G,font->color.B,font->color.A);

        glTexCoord2f (tPoints[(int)(*bf)][0] + font->tIncX, tPoints[(int)(*bf)][1] + font->tIncY);
		glVertex3f (x + size, y + size,0);

        glTexCoord2f (tPoints[(int)(*bf)][0], tPoints[(int)(*bf)][1] + font->tIncY);
		glVertex3f (x , y + size,0);
		glEnd ();
	}
}
/*
=============
fontDrawString

Renders a string at xpos, ypos.
=============
*/
void 
fontDrawString (glCompText* font, GLfloat xpos, GLfloat ypos, 
    GLfloat width, char *s)
{
    int vPort[4];
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


	fontWalkString (font,s, xpos, ypos, vPort,(float)width);
	
/*	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();*/

	fontSetModes (FONT_RESTORE_MODES);
    fontReset (font);
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
void fontMakeMap (glCompText* font)
{
#define VCOPY(d,x,y) {d[0] = x; d[1] = y;}
	int i = 0;
	float x, y;


	font->tIncX = (float)pow (C_DPI, -1);
	font->tIncY = (float)pow (R_DPI, -1);

	
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


glCompText* font_init()
{
    glCompText* font = NEW(glCompText);

	font->color.R=1.00;
	font->color.G=1.00;
	font->color.B=1.00;
	font->color.A=1.00;


	font->fontheight=12;
	font->tIncX=0.0;
	font->tIncY=0.0;
	font->texId=-1;
	font->fontdesc=(char*)0;

	fontMakeMap (font);

    return font;
}

void copy_font(glCompText* targetfont,const glCompText* sourcefont)
{
	targetfont->color.R=sourcefont->color.R;
	targetfont->color.G=sourcefont->color.G;
	targetfont->color.B=sourcefont->color.B;
	targetfont->color.A=sourcefont->color.A;



	targetfont->fontheight=sourcefont->fontheight;
	targetfont->tIncX=sourcefont->tIncX;
	targetfont->tIncY=sourcefont->tIncY;
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
static size_t fontpathsz = 0;

int add_font(fontset_t* fontset,char* fontdesc)
{
    int id;	
    size_t sz;
    glCompText* tf;

    id=fontId(fontset,fontdesc);

    if (id==-1) {
	sz = strlen(fontset->font_directory)+strlen(fontdesc)+6;
	if (sz > fontpathsz) {
	    fontpathsz = 2*sz;
	    fontpath = ALLOC (fontpathsz, fontpath, char); 
        }
	sprintf(fontpath,"%s/%s.png",fontset->font_directory,fontdesc);
	if(create_font_file(fontdesc,fontpath,(float)32,(float)32)==0) {
	    fontset->fonts = ALLOC(fontset->count+1,fontset->fonts,glCompText*);
	    fontset->fonts[fontset->count] = tf = font_init ();
	    tf->fontdesc = strdup(fontdesc);
	    glGenTextures (1, &(tf->texId));	//get  opengl texture name
	    if ((tf->texId >= 0) && glCompLoadFontPNG (fontpath, tf->texId)) {
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

void fontColor (glCompText* font,float r, float g, float b,float a)
{

	
	font->color.R=r;
	font->color.G=g;
	font->color.B=b;
	font->color.A=a;
}
