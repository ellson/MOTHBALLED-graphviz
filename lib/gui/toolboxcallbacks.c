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


void btnToolSingleSelect_clicked(GtkWidget *widget,gpointer user_data)
{
	deselect_all(view.g[view.activeGraph]);
	switch_Mouse (NULL,3);	
}
void btnToolRectSelect_clicked(GtkWidget *widget,gpointer user_data)
{

	deselect_all(view.g[view.activeGraph]);
	view.Selection.Anti=0;
	switch_Mouse (NULL,4);	
}

void btnToolRectXSelect_clicked (GtkWidget *widget,gpointer user_data)
{
	deselect_all(view.g[view.activeGraph]);
	view.Selection.Anti=0;
	switch_Mouse (NULL,5);	
}

void btnToolAntiRectSelect_clicked                       (GtkWidget *widget,gpointer user_data)
{
	view.Selection.Anti=1;
	switch_Mouse (NULL,4);	
	
}
void btnToolAntiRectXSelect_clicked                       (GtkWidget *widget,gpointer user_data)
{
	view.Selection.Anti=1;
	switch_Mouse (NULL,5);	

}


void btnToolPan_clicked                       (GtkWidget *widget,gpointer user_data)
{
	switch_Mouse (NULL,0);	
}
void btnToolZoom_clicked                       (GtkWidget *widget,gpointer user_data)
{
	switch_Mouse (NULL,1);	
}
void btnToolZoomIn_clicked                       (GtkWidget *widget,gpointer user_data)
{
	view.zoom = view.zoom + ZOOM_STEP;
	if(view.zoom > MAX_ZOOM)
		view.zoom=MAX_ZOOM;
	expose_event (drawing_area,NULL,NULL);

}
void btnToolZoomOut_clicked                       (GtkWidget *widget,gpointer user_data)
{
	view.zoom = view.zoom - ZOOM_STEP;
	if(view.zoom < MIN_ZOOM)
		view.zoom=MIN_ZOOM;
	expose_event (drawing_area,NULL,NULL);
}
void btnToolFit_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolFit_clicked\n");
}
void btnToolMove_clicked                       (GtkWidget *widget,gpointer user_data)
{
	switch_Mouse (NULL,10);	
}
void btnToolAddNode_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolAddNode_clicked\n");
}
void btnToolDeleteNode_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolDeleteNode_clicked\n");
}
void btnToolFindNode_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolFindNode_clicked\n");
}
void btnToolAddEdge_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolAddEdge_clicked\n");
}
void btnToolDeleteEdge_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolDeleteEdge_clicked\n");
}
void btnToolFindEdge_clicked                       (GtkWidget *widget,gpointer user_data)
{
	printf("btnToolFindEdge_clicked\n");
	printf("zoom :%f\n",view.zoom);
}
