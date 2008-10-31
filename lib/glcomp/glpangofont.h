
#ifndef GLPANGOFONT_H
#define GLPANGOFONT_H
#include <cairo.h>
#include <pango/pangocairo.h>
#include <png.h>
//creates a font file with given name and font description
//returns non-zero if fails
int create_font_file(char* fontdescription,char* fontfile,float gw,float gh);
#endif

