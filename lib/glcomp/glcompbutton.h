#ifndef GLCOMPBUTTON_H
#define GLCOMPBUTTON_H

#include "glcompdefs.h"


extern glCompButton *glCompButtonNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,char *caption, char *glyphfile,int glyphwidth, int glyphheight,glCompOrientation orientation);
extern int glCompSetAddButton(glCompSet * s, glCompButton * p);
extern int glCompSetRemoveButton(glCompSet * s, glCompButton * p);
extern int glCompDrawButton(glCompButton * p);
extern void glCompButtonClick(glCompButton * p);



#endif
