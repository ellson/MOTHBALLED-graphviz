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

#ifndef TOPVIEWSETTINGS_H
#define TOPVIEWSETTINGS_H

#include "smyrnadefs.h"

_BB void on_settingsOKBtn_clicked(GtkWidget * widget, gpointer user_data);
_BB void on_settingsCancelBtn_clicked(GtkWidget * widget,
				      gpointer user_data);
extern int load_settings_from_graph(Agraph_t * g);
extern int update_graph_from_settings(Agraph_t * g);
extern int show_settings_form();

#endif
