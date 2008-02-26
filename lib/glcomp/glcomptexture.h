/*Open GL texture handling and storing mechanism
  includes glPanel,glCompButton,glCompCustomButton,clCompLabel,glCompStyle
*/
#ifndef GLCOMPTEXTURE_H
#define GLCOMPTEXTURE_H
#include "gltemplate.h"
typedef struct{
	int id;
	GLfloat w,h;
} glCompTexture;

glCompTexture* glCompCreateTextureFromRaw(char* filename,int width,int height,int wrap);
int glCompDeleteTexture(glCompTexture* t);
#endif