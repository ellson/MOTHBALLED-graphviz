/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "toolboxcallbacks.h"
#include "viewport.h"
#include "selection.h"
#include "gltemplate.h"
#include "glutils.h"


void btnToolSingleSelect_clicked(GtkWidget * widget, gpointer user_data)
{
    deselect_all(view->g[view->activeGraph]);
    switch_Mouse(NULL, 3);
}

void btnToolRectSelect_clicked(GtkWidget * widget, gpointer user_data)
{

    deselect_all(view->g[view->activeGraph]);
    view->Selection.Anti = 0;
    switch_Mouse(NULL, 4);
}

void btnToolRectXSelect_clicked(GtkWidget * widget, gpointer user_data)
{
    deselect_all(view->g[view->activeGraph]);
    view->Selection.Anti = 0;
    switch_Mouse(NULL, 5);
}

void btnToolAntiRectSelect_clicked(GtkWidget * widget, gpointer user_data)
{
    view->Selection.Anti = 1;
    switch_Mouse(NULL, 4);

}

void btnToolAntiRectXSelect_clicked(GtkWidget * widget, gpointer user_data)
{
    view->Selection.Anti = 1;
    switch_Mouse(NULL, 5);

}


void btnToolPan_clicked(GtkWidget * widget, gpointer user_data)
{


	switch_Mouse(NULL, 0);

}

void btnToolZoom_clicked(GtkWidget * widget, gpointer user_data)
{
    switch_Mouse(NULL, 1);
}

void btnToolZoomIn_clicked(GtkWidget * widget, gpointer user_data)
{
	view->zoom = view->zoom + ZOOM_STEP*GetOGLDistance(250);
    if (view->zoom > MAX_ZOOM)
	view->zoom = (float) MAX_ZOOM;
    glexpose();

}

void btnToolZoomOut_clicked(GtkWidget * widget, gpointer user_data)
{
	view->FontSizeConst=GetOGLDistance(14);
	view->zoom = view->zoom - ZOOM_STEP*GetOGLDistance(250);
    if (view->zoom < MIN_ZOOM)
	view->zoom = MIN_ZOOM;
    expose_event(view->drawing_area, NULL, NULL);
}

void btnToolZoomFit_clicked(GtkWidget * widget, gpointer user_data)
{

	float z,GDX,SDX,GDY,SDY;
	(view->active_camera >=0)
			? (z=view->cameras[view->active_camera]->r):(z=view->zoom*-1);

	GDX=(view->bdxRight/z-view->bdxLeft/z);
	SDX=(view->clipX2 -view->clipX1);
	GDY=(view->bdyTop/z-view->bdyBottom/z);
	SDY=(view->clipY2 -view->clipY1);
	
	if ((SDX / GDX) <= (SDY / GDY))
	{
		(view->active_camera >=0) ?
			(view->cameras[view->active_camera]->r=view->cameras[view->active_camera]->r/ (SDX/GDX) ):
				(view->zoom = view->zoom /(SDX/GDX));
	}
	else
	{
		(view->active_camera >=0) ?
			(view->cameras[view->active_camera]->r=view->cameras[view->active_camera]->r/ (SDY/GDY) ):
				(view->zoom = view->zoom /(SDY/GDY));

	}
	btnToolFit_clicked(NULL,NULL);
}

void btnToolFit_clicked(GtkWidget * widget, gpointer user_data)
{
	float scx,scy,gcx,gcy,z;
	printf ("graph boundry summary\n");
	printf ("---------------------\n");
	printf ("G   (%f,%f) - (%f,%f)\n",view->bdxLeft/view->zoom*-1,view->bdyBottom/view->zoom
		*-1,view->bdxRight/view->zoom*-1,view->bdyTop/view->zoom*-1);
	printf ("Scr (%f,%f) - (%f,%f)\n",view->clipX1 ,view->clipY1 ,view->clipX2 ,view->clipY2);
	printf ("Old Panx:%f\n",view->panx);


	(view->active_camera >=0)
			? (z=view->cameras[view->active_camera]->r):(z=view->zoom*-1);



	printf ("Z:%f  BDX:%f zoom * BDX :%f\n",z,(view->bdxRight/z-view->bdxLeft/z),z*(view->bdxRight/z-view->bdxLeft/z));





	gcx=view->bdxLeft/z+(view->bdxRight/z-view->bdxLeft/z)/(float)(2.0);
	scx=view->clipX1+(view->clipX2 -view->clipX1)/(float)(2.0);
	gcy=view->bdyBottom/z+(view->bdyTop/z-view->bdyBottom/z)/(float)(2.0);
	scy=view->clipY1+(view->clipY2 -view->clipY1)/(float)(2.0);



	if (view->active_camera >=0)
	{

		view->cameras[view->active_camera]->targetx +=(gcx-scx);
		view->cameras[view->active_camera]->targety +=(gcx-scy);
	}
	else
	{
		float GDX=(view->bdxRight/z-view->bdxLeft/z);
		float SDX=(view->clipX2 -view->clipX1);
		printf ("GDX:%f SDX:%f \n",GDX,SDX);
		view->panx += (gcx-scx);
		view->pany += (gcy-scy);
	}

	printf ("scx:%f\n",scx);
	printf ("gcx:%f\n",gcx);
	printf ("New Panx:%f\n",view->panx);

	glexpose();
}
void btnToolMove_clicked(GtkWidget * widget, gpointer user_data)
{
    switch_Mouse(NULL, 10);
}

void btnToolAddNode_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolDeleteNode_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolFindNode_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolAddEdge_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolDeleteEdge_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolFindEdge_clicked(GtkWidget * widget, gpointer user_data)
{
}
void btnToolCursor_clicked(GtkWidget * widget, gpointer user_data)
{
 switch_Mouse(NULL, -1);
}
void write_to_console(char* text)
{
	//init console text
	GtkTextIter iter;
	if(!view->consoleText)
		view->consoleText=gtk_text_buffer_new(NULL);
	gtk_text_buffer_get_end_iter(view->consoleText,&iter);
	gtk_text_buffer_insert (view->consoleText,&iter,text,-1);
	gtk_text_view_set_buffer ((GtkTextView*)glade_xml_get_widget(xml, "consoleText"),view->consoleText);
}
