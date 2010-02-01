#ifndef SMYRNA_UTILS_H
#define SMYRNA_UTILS_H

#include "smyrnadefs.h"
#include "cgraph.h"

extern int l_int(void *obj, Agsym_t * attr, int def);
extern float l_float(void *obj, Agsym_t * attr, float def);
extern int getAttrBool(Agraph_t* g,void* obj,char* attr_name,int def);
extern int getAttrInt(Agraph_t* g,void* obj,char* attr_name,int def);
extern float getAttrFloat(Agraph_t* g,void* obj,char* attr_name,float def);
extern char* getAttrStr(Agraph_t* g,void* obj,char* attr_name,char* def);
extern void setColor(glCompColor* c,GLfloat R,GLfloat G,GLfloat B,GLfloat A);
extern void getcolorfromschema(colorschemaset * sc, float l, float maxl,glCompColor * c);

#endif
