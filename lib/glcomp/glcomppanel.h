#ifndef GLCOMPPANEL_H
#define GLCOMPPANEL_H

#include "glcompdefs.h"


extern glCompPanel *glCompPanelNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,glCompOrientation orientation);
extern int glCompDrawPanel(glCompPanel * p);
extern int glCompSetAddPanel(glCompSet * s, glCompPanel * p);
extern int glCompSetRemovePanel(glCompSet * s, glCompPanel * p);
extern int glCompPanelShow(glCompPanel * p);
extern int glCompPanelHide(glCompPanel * p);

#endif
