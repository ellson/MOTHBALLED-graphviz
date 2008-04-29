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

#ifndef TOPVIEWDATA_H
#define TOPVIEWDATA_H

#include <gtk/gtk.h>
#include "cgraph.h"
#include "smyrnadefs.h"
#include "tvnodes.h"

int prepare_nodes_for_groups(topview * t, topviewdata * td,
			     int groupindex);
int load_host_buttons(topview * t, Agraph_t * g, glCompSet * s);
int click_group_button(int groupindex);
void glhost_button_clicked_Slot(void *p);
_BB void host_button_clicked_Slot(GtkWidget * widget, gpointer user_data);


#endif
