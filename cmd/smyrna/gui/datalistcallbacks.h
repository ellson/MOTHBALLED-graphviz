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

#ifndef DATALISTCALLBACKS_H
#define DATALISTCALLBACKS_H


#include "gui.h"
#include "tvnodes.h"




_BB void btnTVEdit_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVDelete_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVFilter_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVFirst_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVPrevious_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVNext_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVLast_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVGotopage_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVCancel_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVOK_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVReverse_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void cgbTVSelect_toggled_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVFilterApply_clicked_cb(GtkWidget * widget,
				     gpointer user_data);
_BB void btnTVFilterClear_clicked_cb(GtkWidget * widget,
				     gpointer user_data);
_BB void btnTVSelectAll_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVUnselectAll_clicked_cb(GtkWidget * widget,
				     gpointer user_data);
_BB void btnTVHighlightAll_clicked_cb(GtkWidget * widget,
				      gpointer user_data);
_BB void btnTVUnhighlightAll_clicked_cb(GtkWidget * widget,
					gpointer user_data);
_BB void cgbTVSelect_toggled_cb(GtkWidget * widget, gpointer user_data);
_BB void cgbTVVisible_toggled_cb(GtkWidget * widget, gpointer user_data);
_BB void cgbTVHighlighted_toggled_cb(GtkWidget * widget,
				     gpointer user_data);
_BB void btnTVShowAll_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void btnTVHideAll_clicked_cb(GtkWidget * widget, gpointer user_data);



#endif
