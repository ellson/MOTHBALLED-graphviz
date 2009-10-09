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
#include "glmotion.h"



void btnToolSingleSelect_clicked(GtkWidget * widget, gpointer user_data)
{
    deselect_all(view->g[view->activeGraph]);
    //gtk_toggle_tool_button_set_active   ((GtkToggleToolButton*)widget,1);
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
    glmotion_zoom_inc(1);
}

void btnToolZoomOut_clicked(GtkWidget * widget, gpointer user_data)
{
    glmotion_zoom_inc(0);
}

void btnToolZoomFit_clicked(GtkWidget * widget, gpointer user_data)
{

    float z, GDX, SDX, GDY, SDY;
    (view->active_camera >= 0)
	? (z = view->cameras[view->active_camera]->r) : (z =
							 view->zoom * -1);

    GDX = (view->bdxRight / z - view->bdxLeft / z);
    SDX = (view->clipX2 - view->clipX1);
    GDY = (view->bdyTop / z - view->bdyBottom / z);
    SDY = (view->clipY2 - view->clipY1);

    if ((SDX / GDX) <= (SDY / GDY)) {
	(view->active_camera >= 0) ?
	    (view->cameras[view->active_camera]->r =
	     view->cameras[view->active_camera]->r / (SDX /
						      GDX)) : (view->zoom =
							       view->zoom /
							       (SDX /
								GDX));
    } else {
	(view->active_camera >= 0) ?
	    (view->cameras[view->active_camera]->r =
	     view->cameras[view->active_camera]->r / (SDY /
						      GDY)) : (view->zoom =
							       view->zoom /
							       (SDY /
								GDY));

    }
    btnToolFit_clicked(NULL, NULL);
}

void btnToolFit_clicked(GtkWidget * widget, gpointer user_data)
{
    float scx, scy, gcx, gcy, z;



    (view->active_camera >= 0)
	? (z = view->cameras[view->active_camera]->r) : (z =
							 view->zoom * -1);





    gcx =
	view->bdxLeft / z + (view->bdxRight / z -
			     view->bdxLeft / z) / (float) (2.0);
    scx = view->clipX1 + (view->clipX2 - view->clipX1) / (float) (2.0);
    gcy =
	view->bdyBottom / z + (view->bdyTop / z -
			       view->bdyBottom / z) / (float) (2.0);
    scy = view->clipY1 + (view->clipY2 - view->clipY1) / (float) (2.0);



    if (view->active_camera >= 0) {

	view->cameras[view->active_camera]->targetx += (gcx - scx);
	view->cameras[view->active_camera]->targety += (gcx - scy);
    } else {
	view->panx += (gcx - scx);
	view->pany += (gcy - scy);
    }
    view->Topview->fitin_zoom = view->zoom;

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
void write_to_console(char *text)
{
    //init console text
    GtkTextIter iter;
    if (!view->consoleText)
	view->consoleText = gtk_text_buffer_new(NULL);
    gtk_text_buffer_get_end_iter(view->consoleText, &iter);
    gtk_text_buffer_insert(view->consoleText, &iter, text, -1);
    gtk_text_view_set_buffer((GtkTextView *)
			     glade_xml_get_widget(xml, "consoleText"),
			     view->consoleText);
}

void on_btnActivateGraph_clicked(GtkWidget * widget, gpointer user_data)
{
    int graphId;
    graphId = gtk_combo_box_get_active(view->graphComboBox);
    /* fprintf (stderr, "switch to graph %d\n",graphId); */
    switch_graph(graphId);
}
