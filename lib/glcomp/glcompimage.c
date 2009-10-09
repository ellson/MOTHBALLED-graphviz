/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/
#include "glcompimage.h"
#include "glcompfont.h"
#include "glcompset.h"
#include "glutils.h"
#include "glcomptexture.h"

glCompImage *glCompImageNew(glCompObj* par,GLfloat x, GLfloat y)
{
    glCompImage *p;
    p = malloc(sizeof(glCompImage));
	glCompInitCommon((glCompObj*)p,par,x,y);
	p->objType=glImageObj;
	//typedef enum {glPanelObj,glbuttonObj,glLabelObj,glImageObj}glObjType;

	p->objType=glImageObj;
	p->stretch=0;
	p->pngFile=(char*)0;
	p->texture=NULL;
	p->common.functions.draw=glCompImageDraw;
    return p;
}
void glCompImageDelete (glCompImage* p)
{
	glCompEmptyCommon(&p->common);
	if(p->pngFile)
		free (p->pngFile);
	if(p->texture)
		glCompDeleteTexture(p->texture);
	free(p);
}
extern unsigned char *load_png(char *file_name, int *imageWidth,int *imageHeight);
int glCompImageLoad(glCompImage* i,unsigned char* data,int width,int height)
{
	if(data != NULL)	/*valid image data*/
	{
		glCompDeleteTexture(i->texture);
		i->texture=glCompSetAddNewTexImage(i->common.compset,width,height,data,1);
		if(i->texture)
		{
			i->common.width=width;
			i->common.height=height;
			return 1;
		}

	}
	return 0;
}



int glCompImageLoadPng(glCompImage* i,char* pngFile)
{
	int imageWidth,imageHeight;
	unsigned char* data;
	data=load_png(pngFile, &imageWidth, &imageHeight);
	return glCompImageLoad(i,data,imageWidth,imageHeight);
}
int glCompImageLoadRaw(glCompSet * s,glCompImage* i,char* rawFile)
{
	int imageWidth,imageHeight;
	unsigned char* data;
	data=load_png(rawFile, &imageWidth, &imageHeight);
	return glCompImageLoad(i,data,imageWidth,imageHeight);
}

void glCompImageDraw(void* obj)
{
	glCompImage* p;
	static glCompCommon ref;
//	static GLfloat w,h,d;
	p=(glCompImage*)obj;
	ref=p->common;
	glCompCalcWidget((glCompCommon*)p->common.parent,&p->common,&ref);
    if (!p->common.visible)
		return ;
	if (!p->texture)
		return;
	glRasterPos2f(ref.pos.x,ref.pos.y);
	glDrawPixels(p->texture->width,p->texture->height,GL_RGBA,GL_UNSIGNED_BYTE,p->texture->data);

/*	w=ref.width;
	h=ref.height;
	d=(GLfloat)p->common.layer* (GLfloat)GLCOMPSET_BEVEL_DIFF;
    glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D,p->texture->id);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0f, 1.0f);glVertex3d(ref.pos.x,ref.pos.y,d);
		glTexCoord2d(1.0f, 1.0f);glVertex3d(ref.pos.x+w,ref.pos.y,d);
		glTexCoord2d(1.0f, 0.0f);glVertex3d(ref.pos.x+w,ref.pos.y+h,d);
		glTexCoord2d(0.0f, 0.0f);glVertex3d(ref.pos.x,ref.pos.y+h,d);
	glEnd();

	glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);*/

}

void glCompImageClick(glCompObj * o,GLfloat x,GLfloat y,glMouseButtonType t)
{
	if (o->common.callbacks.click)
		o->common.callbacks.click(o,x,y,t);	
}

void glCompImageDoubleClick(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.doubleclick)
	((glCompImage*)obj)->common.callbacks.doubleclick(obj,x,y,t);
}

void glCompImageMouseDown(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.mousedown)
		((glCompImage*)obj)->common.callbacks.mousedown(obj,x,y,t);
}

void glCompImageMouseIn(glCompObj * obj,GLfloat x,GLfloat y)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.mousein)
		((glCompImage*)obj)->common.callbacks.mousein(obj,x,y);
}
void glCompImageMouseOut(glCompObj * obj,GLfloat x,GLfloat y)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.mouseout)
		((glCompImage*)obj)->common.callbacks.mouseout(obj,x,y);
}
void glCompImageMouseOver(glCompObj * obj,GLfloat x,GLfloat y)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.mouseover)
		((glCompImage*)obj)->common.callbacks.mouseover(obj,x,y);
}
void glCompImageMouseUp(glCompObj * obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	/*Put your internal code here*/
	if (((glCompImage*)obj)->common.callbacks.mouseup)
		((glCompImage*)obj)->common.callbacks.mouseup(obj,x,y,t);
}

