#ifndef GLCOMPLABEL_H
#define GLCOMPLABEL_H

#include "glcompdefs.h"


extern glCompLabel *glCompLabelNew(GLfloat x, GLfloat y, GLfloat size, char *text,glCompOrientation orientation);
extern int glCompSetAddLabel(glCompSet * s, glCompLabel * p);
extern int glCompSetRemoveLabel(glCompSet * s, glCompLabel * p);
extern int glCompDrawLabel(glCompLabel * p);



#endif
