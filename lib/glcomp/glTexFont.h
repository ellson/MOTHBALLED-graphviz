#ifndef GLCOMPTEXT_H
#define GLTEXFONTH_H

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


#ifndef vec2_t
typedef float vec2_t[2];
#endif


#ifdef __cplusplus
extern "C" {
#endif
extern fontset_t* fontset_init(void);
void free_font_set(fontset_t* fontset);
void copy_font(glCompText* targetfont,const glCompText* sourcefont);
int add_font(fontset_t* fontset,char* fontdesc);
int glCompLoadFont (glCompText* font,char *name);
void fontDrawString (glCompText*, GLfloat , GLfloat , GLfloat, char*);
void fontSize (glCompText* font,GLfloat size);
void fontzdepth(glCompText* font,float zdepth);
glCompText* font_init(void);
void fontColor (glCompText* font,float r, float g, float b,float a);

void fontDrawChar (glCompText*,char, GLfloat, GLfloat, GLfloat);
void fontScissorNormal (glCompText* font,int xpos, int ypos, int tabs, int carrage, int size, int len);
int fontSetColorFromToken (glCompText* font,char *s);
int fontGetCharHits (char *s, char f);
void fontMakeMap (glCompText* font);
void fontSetModes (int state);
void fontReset (glCompText* font);
int fontItalicsMode (glCompText* font,char *s);
int fontBoldMode (glCompText* font,char *s);
void fontRenderChar (glCompText* font,char c, GLfloat x, GLfloat y, GLfloat size);
#ifdef __cplusplus
}
#endif

#endif


